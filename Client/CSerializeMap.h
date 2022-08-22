#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

template<class Key, class Value>
class serializable_map : public std::map<Key, Value> {
private:
    int offset{};
    std::vector<Key> restore;

    template<class T>
    void write(std::stringstream& ss, T& str)
    {
        //auto str1 = (std::string)STRING(str);
        size_t size = str.size();
       // auto a = (char*)(&size);
       // auto b = (char*)(str.data());
       // auto c = sizeof(size);
       // auto d = str.length();
        ss.write((char*)(&size), sizeof(size));
        ss.write((char*)(str.data()), str.length());
    }

    template<>
    void write(std::stringstream& ss, int& str)
    {
        auto str1 = (std::to_string(str));
        size_t size = str1.size();
       // auto a = (char*)(&size);
       // auto b = (char*)(str1.data());
       // auto c = sizeof(size);
       // auto d = str1.length();
        ss.write((char*)(&size), sizeof(size));
        ss.write((char*)(str1.data()), str1.length());

    }


    template<class T>
    void read(std::vector<char>& buffer, T& str)
    {

        // auto str1 = (std::to_string(str));

        size_t size = (int)(*(buffer.data() + offset));
        offset += sizeof(size_t);
        std::string str2(buffer.data() + offset, buffer.data() + offset + size);
        str = str2;
        offset += size;
       // std::cout << "\t" << "Value is";
       // std::cout << str << std::endl;
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



       // std::cout << "\t" << "Value is";
       // std::cout << str1 << std::endl;
    }





public:
    //static Key newKey[500];
    std::vector<char> serialize() {

        std::map<Key, Value> newMap;
        std::vector<char> buffer;
        std::stringstream ss;
        int cnt = 0;
        //newKey = new Key[*this];
        for (auto& i : (*this)) {

            Key str = i.first;

            //newKey[cnt] = i.first;
            //restore[cnt] = str;
            Value value = i.second;
            newMap.insert(std::pair<Key, Value>(str, value));
            write(ss, str);
            write(ss, value);

            //std::cout << "MyConter from ser " << cnt << '\n';
            //std::cout << (newMap[str]);
            ++cnt;

        }
        size_t size = ss.str().size();
        buffer.resize(size);
        ss.read(buffer.data(), size);
        return buffer;
    }
    void deserialize(std::vector<char>& buffer)
    {
        //std::map<Key, Value> newMap;


        offset = 0;
        int cnt = 0;
        while (offset < buffer.size())

        {

            Key key{};
            // int a = (int)key;
            Value value{};
            auto a = (std::string)TOSTRING(key);
            read(buffer, key);
            read(buffer, value);
            (*this)[key] = value;
            //mp[key] = value;

           //newMap.insert(std::pair<Key, Value>(key, value));


          // mp.insert(std::pair<Key, Value>(key, value));

           // std::cout << "Ofset from des func -> " << "\t" << offset << std::endl;
            //auto j = (std::string)value;
           // std::cout << "MyConter  from des #" << cnt << '\n';
           // std::cout << (newMap[key]);
            ++cnt;

        }

    }

};

//template<class Key, class Value>
//Key serializable_map<Key, Value>::newKey[500]{};




