#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

//class for serialize/deserialize map
template<class Key, class Value>
class serializable_map : public std::map<Key, Value> {
private:
    int offset_{};

    template<class T>
    void write(std::stringstream& ss, T& str)
    {
        size_t size = str.size();
        ss.write((char*)(&size), sizeof(size));
        ss.write((char*)(str.data()), str.length());
    }

    //template<>
    void write(std::stringstream& ss, const int& str) const
    {
        auto str1 = (std::to_string(str));
        size_t size = str1.size();
        ss.write((char*)(&size), sizeof(size));
        ss.write((char*)(str1.data()), str1.length());

    }

    template<class T>
    void read(std::vector<char>& buffer, T& str)
    {
        size_t size = (int)(*(buffer.data() + offset_));
        offset_ += sizeof(size_t);
        std::string str2(buffer.data() + offset_, buffer.data() + offset_ + size);
        str = str2;
        offset_ += size;
    }

   // template<>
    void read(std::vector<char>& buffer, int& str)
    {

        auto str1 = (std::to_string(str));

        const size_t size = (int)(*(buffer.data() + offset_));
        offset_ += sizeof(size_t);
        const std::string str2(buffer.data() + offset_, buffer.data() + offset_ + size);
        str1 = str2;
        str = std::stoi(str1);
        offset_ += size;
    }

public:

    std::vector<char> serialize() {

        std::map<Key, Value> newMap;
        std::vector<char> buffer;
        std::stringstream ss;
        int cnt = 0;
        for (auto& i : (*this)) {

            Key str = i.first;
            Value value = i.second;
            newMap.insert(std::pair<Key, Value>(str, value));
            write(ss, str);
            write(ss, value);
            ++cnt;

        }
        const size_t size = ss.str().size();
        buffer.resize(size);
        ss.read(buffer.data(), size);
        return buffer;
    }
    void deserialize(std::vector<char>& buffer)
    {
        offset_ = 0;
        int cnt = 0;
        while (offset_ < buffer.size())
        {
            Key key{};
            Value value{};
            read(buffer, key);
            read(buffer, value);
            (*this)[key] = value;
            ++cnt;
        }
    }
};