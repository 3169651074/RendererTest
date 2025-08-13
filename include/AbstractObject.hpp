#ifndef RENDERERTEST_ABSTRACTOBJECT_HPP
#define RENDERERTEST_ABSTRACTOBJECT_HPP

#include <Global.hpp>

namespace renderer {
    //对象基类，抽象类
    class AbstractObject {
    public:
        static constexpr Uint32 TOSTRING_BUFFER_SIZE = 200;

        //虚析构函数
        virtual ~AbstractObject() = default;

        //判断对象是否相等
        virtual bool equals(const AbstractObject & obj) const = 0;

        //转换对象为字符串
        virtual std::string toString() const = 0;

        //输出流重载
        friend std::ostream & operator<<(std::ostream & os, const AbstractObject & obj) {
            return os << obj.toString();
        }

        //比较运算符
        bool operator==(const AbstractObject & obj) const {
            return equals(obj);
        }
        bool operator!=(const AbstractObject & obj) const {
            return !equals(obj);
        }
    };
}

#endif //RENDERERTEST_ABSTRACTOBJECT_HPP
