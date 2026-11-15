#pragma once
#ifndef DISCORD_MINIMAL_JSON_HPP
#define DISCORD_MINIMAL_JSON_HPP

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace discord::json {

// ============================================================
// JSON Writer - builds a JSON string incrementally
// ============================================================

class Writer {
public:
    Writer& beginObject() { comma(); m_buf += '{'; m_first = true; return *this; }
    Writer& endObject()   { m_buf += '}'; m_first = false; return *this; }
    Writer& beginArray()  { comma(); m_buf += '['; m_first = true; return *this; }
    Writer& endArray()    { m_buf += ']'; m_first = false; return *this; }

    Writer& key(std::string_view k) {
        comma();
        m_buf += '"';
        escape(k);
        m_buf += "\":";
        m_first = true;
        return *this;
    }

    Writer& string(std::string_view v) {
        comma();
        m_buf += '"';
        escape(v);
        m_buf += '"';
        m_first = false;
        return *this;
    }

    Writer& number(int64_t v) {
        comma();
        m_buf += std::to_string(v);
        m_first = false;
        return *this;
    }

    Writer& number_double(double v) {
        comma();
        m_buf += formatDouble(v);
        m_first = false;
        return *this;
    }

    Writer& boolean(bool v) {
        comma();
        m_buf += v ? "true" : "false";
        m_first = false;
        return *this;
    }

    Writer& null() {
        comma();
        m_buf += "null";
        m_first = false;
        return *this;
    }

    std::string take() { return std::move(m_buf); }

    // Format a double with shortest round-trip representation
    static std::string formatDouble(double d) {
        if (std::isnan(d) || std::isinf(d)) return "null";
        char buf[64];
        auto cleanup = [](std::string s) {
            // Remove '+' from positive exponents (e.g. e+308 -> e308)
            auto pos = s.find("e+");
            if (pos == std::string::npos) pos = s.find("E+");
            if (pos != std::string::npos) {
                s.erase(pos + 1, 1);
            }
            // Ensure it looks like a double (has '.' or 'e')
            if (s.find('.') == std::string::npos && s.find('e') == std::string::npos
                && s.find('E') == std::string::npos) {
                s += ".0";
            }
            return s;
        };
        // Try increasing precision until round-trip is exact
        for (int prec = 1; prec <= 17; ++prec) {
            int len = std::snprintf(buf, sizeof(buf), "%.*g", prec, d);
            // Verify round-trip
            char* end = nullptr;
            double rt = std::strtod(buf, &end);
            if (rt == d) {
                return cleanup(std::string(buf, static_cast<size_t>(len)));
            }
        }
        // Fallback with max precision
        int len = std::snprintf(buf, sizeof(buf), "%.17g", d);
        return cleanup(std::string(buf, static_cast<size_t>(len)));
    }

private:
    void comma() { if (!m_first) m_buf += ','; }

    void escape(std::string_view v) {
        for (size_t i = 0; i < v.size(); ++i) {
            auto c = static_cast<unsigned char>(v[i]);
            switch (c) {
                case '"':  m_buf += "\\\""; break;
                case '\\': m_buf += "\\\\"; break;
                case '\b': m_buf += "\\b"; break;
                case '\f': m_buf += "\\f"; break;
                case '\n': m_buf += "\\n"; break;
                case '\r': m_buf += "\\r"; break;
                case '\t': m_buf += "\\t"; break;
                default:
                    if (c < 0x20) {
                        char buf[8];
                        std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                        m_buf += buf;
                    } else {
                        m_buf += static_cast<char>(c);
                    }
                    break;
            }
        }
    }

    std::string m_buf;
    bool m_first = true;
};

// ============================================================
// JSON Value - lightweight DOM representation
// ============================================================

class Value {
public:
    enum Type { Null, Bool, Integer, Double, String, Object, Array };

    Value() = default;

    Type type() const { return m_type; }
    bool is_null()    const { return m_type == Null; }
    bool is_string()  const { return m_type == String; }
    bool is_number()  const { return m_type == Integer || m_type == Double; }
    bool is_integer() const { return m_type == Integer; }
    bool is_double()  const { return m_type == Double; }
    bool is_bool()    const { return m_type == Bool; }
    bool is_object()  const { return m_type == Object; }
    bool is_array()   const { return m_type == Array; }

    std::string const& str()  const { return m_str; }
    int64_t             num()  const { return m_type == Double ? static_cast<int64_t>(m_dbl) : m_int; }
    double              dbl()  const { return m_type == Integer ? static_cast<double>(m_int) : m_dbl; }
    bool                bval() const { return m_bool; }

    Value const* get(std::string const& key) const {
        if (m_type != Object) return nullptr;
        auto it = m_obj.find(key);
        return it != m_obj.end() ? &it->second : nullptr;
    }

    void set_null()                { m_type = Null; }
    void set_bool(bool v)          { m_type = Bool;    m_bool = v; }
    void set_integer(int64_t v)    { m_type = Integer; m_int = v; }
    void set_double(double v)      { m_type = Double;  m_dbl = v; }
    void set_number(int64_t v)     { m_type = Integer; m_int = v; }  // alias
    void set_string(std::string s) { m_type = String;  m_str = std::move(s); }
    void set_object()              { m_type = Object; }
    void set_array()               { m_type = Array; }

    std::map<std::string, Value>&       obj()       { return m_obj; }
    std::vector<Value>&                 arr()       { return m_arr; }
    std::map<std::string, Value> const& obj() const { return m_obj; }
    std::vector<Value> const&           arr() const { return m_arr; }

private:
    Type m_type = Null;
    bool m_bool = false;
    int64_t m_int = 0;
    double m_dbl = 0.0;
    std::string m_str;
    std::map<std::string, Value> m_obj;
    std::vector<Value> m_arr;
};

// ============================================================
// JSON Parser - recursive-descent, RFC 8259 compliant
// ============================================================

class Parser {
public:
    bool parse(std::string_view input, Value& out) {
        m_input = input;
        m_pos = 0;
        if (!parseValue(out)) return false;
        skipWS();
        // Must consume entire input (no trailing content)
        if (m_pos != m_input.size()) return false;
        return true;
    }

private:
    std::string_view m_input;
    size_t m_pos = 0;

    char peek() const { return m_pos < m_input.size() ? m_input[m_pos] : '\0'; }
    char next()       { return m_pos < m_input.size() ? m_input[m_pos++] : '\0'; }

    void skipWS() {
        while (m_pos < m_input.size()) {
            char c = m_input[m_pos];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') ++m_pos;
            else break;
        }
    }

    bool expect(char c) {
        skipWS();
        if (peek() == c) { ++m_pos; return true; }
        return false;
    }

    bool parseValue(Value& out) {
        skipWS();
        char c = peek();
        if (c == '"')                           return parseString(out);
        if (c == '{')                           return parseObject(out);
        if (c == '[')                           return parseArray(out);
        if (c == 't' || c == 'f')               return parseBool(out);
        if (c == 'n')                           return parseNull(out);
        if (c == '-' || (c >= '0' && c <= '9')) return parseNumber(out);
        return false;
    }

    bool parseString(Value& out) {
        std::string s;
        if (!readString(s)) return false;
        out.set_string(std::move(s));
        return true;
    }

    // Read a 4-digit hex value for \uXXXX
    bool readHex4(uint32_t& codepoint) {
        if (m_pos + 4 > m_input.size()) return false;
        codepoint = 0;
        for (int i = 0; i < 4; ++i) {
            char c = m_input[m_pos++];
            codepoint <<= 4;
            if (c >= '0' && c <= '9')      codepoint |= static_cast<uint32_t>(c - '0');
            else if (c >= 'a' && c <= 'f') codepoint |= static_cast<uint32_t>(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') codepoint |= static_cast<uint32_t>(c - 'A' + 10);
            else return false;
        }
        return true;
    }

    // Encode a Unicode codepoint as UTF-8
    static void encodeUTF8(uint32_t cp, std::string& out) {
        if (cp <= 0x7F) {
            out += static_cast<char>(cp);
        } else if (cp <= 0x7FF) {
            out += static_cast<char>(0xC0 | (cp >> 6));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0xFFFF) {
            out += static_cast<char>(0xE0 | (cp >> 12));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0x10FFFF) {
            out += static_cast<char>(0xF0 | (cp >> 18));
            out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }

    bool readString(std::string& out) {
        if (next() != '"') return false;
        while (m_pos < m_input.size()) {
            char c = m_input[m_pos];
            // Control characters (U+0000 through U+001F) must be escaped
            if (static_cast<unsigned char>(c) < 0x20) return false;
            ++m_pos;
            if (c == '"') return true;
            if (c == '\\') {
                if (m_pos >= m_input.size()) return false;
                char esc = m_input[m_pos++];
                switch (esc) {
                    case '"':  out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/':  out += '/'; break;
                    case 'b':  out += '\b'; break;
                    case 'f':  out += '\f'; break;
                    case 'n':  out += '\n'; break;
                    case 'r':  out += '\r'; break;
                    case 't':  out += '\t'; break;
                    case 'u': {
                        uint32_t cp = 0;
                        if (!readHex4(cp)) return false;
                        // Handle surrogate pairs
                        if (cp >= 0xD800 && cp <= 0xDBFF) {
                            // High surrogate, must be followed by \uXXXX low surrogate
                            if (m_pos + 1 >= m_input.size()) return false;
                            if (m_input[m_pos] != '\\' || m_input[m_pos + 1] != 'u') return false;
                            m_pos += 2;
                            uint32_t low = 0;
                            if (!readHex4(low)) return false;
                            if (low < 0xDC00 || low > 0xDFFF) return false;
                            cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                        } else if (cp >= 0xDC00 && cp <= 0xDFFF) {
                            // Lone low surrogate is invalid
                            return false;
                        }
                        encodeUTF8(cp, out);
                        break;
                    }
                    default: return false; // invalid escape
                }
            } else {
                out += c;
            }
        }
        return false; // unterminated string
    }

    bool parseNumber(Value& out) {
        size_t start = m_pos;
        bool isDouble = false;

        // Optional minus
        if (peek() == '-') ++m_pos;

        // Integer part: RFC 8259 says no leading zeros (except single '0')
        if (m_pos >= m_input.size()) return false;
        char c = m_input[m_pos];
        if (c < '0' || c > '9') return false; // must start with digit

        if (c == '0') {
            ++m_pos;
            // After leading '0', next char must NOT be a digit (no leading zeros)
            if (m_pos < m_input.size()) {
                char nc = m_input[m_pos];
                if (nc >= '0' && nc <= '9') return false; // leading zero
            }
        } else {
            // 1-9 followed by any digits
            ++m_pos;
            while (m_pos < m_input.size() && m_input[m_pos] >= '0' && m_input[m_pos] <= '9')
                ++m_pos;
        }

        // Fractional part
        if (m_pos < m_input.size() && m_input[m_pos] == '.') {
            isDouble = true;
            ++m_pos;
            // Must have at least one digit after '.'
            if (m_pos >= m_input.size() || m_input[m_pos] < '0' || m_input[m_pos] > '9')
                return false;
            while (m_pos < m_input.size() && m_input[m_pos] >= '0' && m_input[m_pos] <= '9')
                ++m_pos;
        }

        // Exponent part
        if (m_pos < m_input.size() && (m_input[m_pos] == 'e' || m_input[m_pos] == 'E')) {
            isDouble = true;
            ++m_pos;
            // Optional sign
            if (m_pos < m_input.size() && (m_input[m_pos] == '+' || m_input[m_pos] == '-'))
                ++m_pos;
            // Must have at least one digit after exponent
            if (m_pos >= m_input.size() || m_input[m_pos] < '0' || m_input[m_pos] > '9')
                return false;
            while (m_pos < m_input.size() && m_input[m_pos] >= '0' && m_input[m_pos] <= '9')
                ++m_pos;
        }

        std::string numStr(m_input.data() + start, m_pos - start);

        if (isDouble) {
            // Parse as double
            char* end = nullptr;
            double d = std::strtod(numStr.c_str(), &end);
            if (end != numStr.c_str() + numStr.size()) return false;
            out.set_double(d);
        } else {
            // Try int64_t first, fallback to double for huge integers
            char* end = nullptr;
            errno = 0;
            long long ll = std::strtoll(numStr.c_str(), &end, 10);
            if (errno == ERANGE || end != numStr.c_str() + numStr.size()) {
                // Too large for int64, parse as double
                errno = 0;
                double d = std::strtod(numStr.c_str(), &end);
                if (end != numStr.c_str() + numStr.size()) return false;
                out.set_double(d);
            } else {
                out.set_integer(static_cast<int64_t>(ll));
            }
        }
        return true;
    }

    bool parseBool(Value& out) {
        if (m_input.size() - m_pos >= 4 && m_input.substr(m_pos, 4) == "true") {
            m_pos += 4; out.set_bool(true); return true;
        }
        if (m_input.size() - m_pos >= 5 && m_input.substr(m_pos, 5) == "false") {
            m_pos += 5; out.set_bool(false); return true;
        }
        return false;
    }

    bool parseNull(Value& out) {
        if (m_input.size() - m_pos >= 4 && m_input.substr(m_pos, 4) == "null") {
            m_pos += 4; out.set_null(); return true;
        }
        return false;
    }

    bool parseObject(Value& out) {
        if (!expect('{')) return false;
        out.set_object();
        skipWS();
        if (peek() == '}') { ++m_pos; return true; }
        do {
            skipWS();
            std::string key;
            if (!readString(key)) return false;
            if (!expect(':')) return false;
            Value val;
            if (!parseValue(val)) return false;
            out.obj()[std::move(key)] = std::move(val);
            skipWS();
        } while (expect(','));
        return expect('}');
    }

    bool parseArray(Value& out) {
        if (!expect('[')) return false;
        out.set_array();
        skipWS();
        if (peek() == ']') { ++m_pos; return true; }
        do {
            Value val;
            if (!parseValue(val)) return false;
            out.arr().push_back(std::move(val));
            skipWS();
        } while (expect(','));
        return expect(']');
    }
};

// ============================================================
// Stringify a Value back to JSON
// ============================================================

inline void stringify(Value const& v, std::string& out) {
    switch (v.type()) {
        case Value::Null:    out += "null"; break;
        case Value::Bool:    out += v.bval() ? "true" : "false"; break;
        case Value::Integer: out += std::to_string(v.num()); break;
        case Value::Double: {
            out += Writer::formatDouble(v.dbl());
            break;
        }
        case Value::String: {
            out += '"';
            for (size_t i = 0; i < v.str().size(); ++i) {
                auto c = static_cast<unsigned char>(v.str()[i]);
                switch (c) {
                    case '"':  out += "\\\""; break;
                    case '\\': out += "\\\\"; break;
                    case '\b': out += "\\b"; break;
                    case '\f': out += "\\f"; break;
                    case '\n': out += "\\n"; break;
                    case '\r': out += "\\r"; break;
                    case '\t': out += "\\t"; break;
                    default:
                        if (c < 0x20) {
                            char buf[8];
                            std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                            out += buf;
                        } else {
                            out += static_cast<char>(c);
                        }
                        break;
                }
            }
            out += '"';
            break;
        }
        case Value::Array: {
            out += '[';
            bool first = true;
            for (auto const& elem : v.arr()) {
                if (!first) out += ',';
                first = false;
                stringify(elem, out);
            }
            out += ']';
            break;
        }
        case Value::Object: {
            out += '{';
            bool first = true;
            for (auto const& kv : v.obj()) {
                if (!first) out += ',';
                first = false;
                // stringify key
                out += '"';
                for (auto c : kv.first) {
                    auto uc = static_cast<unsigned char>(c);
                    switch (uc) {
                        case '"':  out += "\\\""; break;
                        case '\\': out += "\\\\"; break;
                        case '\b': out += "\\b"; break;
                        case '\f': out += "\\f"; break;
                        case '\n': out += "\\n"; break;
                        case '\r': out += "\\r"; break;
                        case '\t': out += "\\t"; break;
                        default:
                            if (uc < 0x20) {
                                char buf[8];
                                std::snprintf(buf, sizeof(buf), "\\u%04x", uc);
                                out += buf;
                            } else {
                                out += c;
                            }
                            break;
                    }
                }
                out += "\":";
                stringify(kv.second, out);
            }
            out += '}';
            break;
        }
    }
}

inline std::string stringify(Value const& v) {
    std::string out;
    stringify(v, out);
    return out;
}

// ============================================================
// Convenience helpers
// ============================================================

inline bool parse(std::string_view input, Value& out) {
    Parser p;
    return p.parse(input, out);
}

/// Extract a string field from a JSON object value
inline void extract(Value const& v, std::string const& key, std::string& out) {
    if (auto* p = v.get(key)) { if (p->is_string()) out = p->str(); }
}

/// Extract an optional string field
inline void extract(Value const& v, std::string const& key, std::optional<std::string>& out) {
    if (auto* p = v.get(key)) {
        if (p->is_string()) out = p->str();
        else if (p->is_null()) out = std::nullopt;
    }
}

/// Extract an int field
inline void extract(Value const& v, std::string const& key, int& out) {
    if (auto* p = v.get(key)) { if (p->is_number()) out = static_cast<int>(p->num()); }
}

/// Extract a uint64_t field
inline void extract(Value const& v, std::string const& key, uint64_t& out) {
    if (auto* p = v.get(key)) { if (p->is_number()) out = static_cast<uint64_t>(p->num()); }
}

/// Extract a bool field
inline void extract(Value const& v, std::string const& key, bool& out) {
    if (auto* p = v.get(key)) { if (p->is_bool()) out = p->bval(); }
}

} // namespace discord::json

#endif // DISCORD_MINIMAL_JSON_HPP
