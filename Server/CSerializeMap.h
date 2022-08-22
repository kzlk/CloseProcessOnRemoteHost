#include <iostream>
#include <vector>
#include <map>
#include <sstream>

//Class for serialize/deserialize map
//TODO: 

template<class Key, class Value>
class serializable_map : public std::map<Key, Value> {
private:
    int offset;
    std::vector<Key> restore;

    template<class T>
    void write(std::stringstream& ss, T& str)
    {

        size_t size = str.size();
        auto a = (char*)(&size);
        auto b = (char*)(str.data());
        auto c = sizeof(size);
        auto d = str.length();
        ss.write((char*)(&size), sizeof(size));
        ss.write((char*)(str.data()), str.length());
    }

    template<>
    void write(std::stringstream& ss, int& str)
    {
        auto str1 = (std::to_string(str));
        size_t size = str1.size();
        auto a = (char*)(&size);
        auto b = (char*)(str1.data());
        auto c = sizeof(size);
        auto d = str1.length();
        ss.write((char*)(&size), sizeof(size));
        ss.write((char*)(str1.data()), str1.length());

    }

    template<class T>
    void read(std::vector<char>& buffer, T& str)
    {
        size_t size = (int)(*(buffer.data() + offset));
        offset += sizeof(size_t);
        std::string str2(buffer.data() + offset, buffer.data() + offset + size);
        str = str2;
        offset += size;
        std::cout << "\t" << "Value is";
        std::cout << str << std::endl;
    }

    template<>
    void read(std::vector<char>& buffer, int& str)
    {

        auto str1 = (std::to_string(str));

        size_t size = (int)(*(buffer.data() + offset));
        offset += sizeof(size_t);
        std::string str2(buffer.data() + offset, buffer.data() + offset + size);
        str1 = str2;
        str = std::stoi(str1);
        offset += size;

        std::cout << "\t" << "Value is";
        std::cout << str1 << std::endl;
    }

public:

    std::vector<char> serialize() {

        std::map<Key, Value> newMap;
        std::vector<char> buffer;
        std::stringstream ss;
        int cnt = 0;
        for (auto& i : (*this)) {

            Key str = i.first;

            //newKey[cnt] = i.first;

            Value value = i.second;
            newMap.insert(std::pair<Key, Value>(str, value));
            write(ss, str);
            write(ss, value);

            ++cnt;

        }
        size_t size = ss.str().size();
        buffer.resize(size);
        ss.read(buffer.data(), size);
        return buffer;
    }
    void deserialize(std::vector<char>& buffer)
    {

        offset = 0;
        int cnt = 0;
        while (offset < buffer.size())
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