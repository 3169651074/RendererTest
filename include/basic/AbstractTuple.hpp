#ifndef RENDERERTEST_ABSTRACTTUPLE_HPP
#define RENDERERTEST_ABSTRACTTUPLE_HPP

#include <AbstractObject.hpp>

namespace renderer {
    /*
     * 三元素对象抽象类，数组元素类型自定义
     */
    template <typename T>
    class AbstractTuple : public AbstractObject {
    protected:
        T elements[3];

    public:
        explicit AbstractTuple(const T & e1, const T & e2, const T & e3) {
            elements[0] = e1; elements[1] = e2; elements[2] = e3;
        }

        explicit AbstractTuple(const T elements[3]) {
            memcpy(this->elements, elements, 3 * sizeof(T));
        }

        ~AbstractTuple() override = default;

        bool equals(const AbstractObject & obj) const override {
            if (this == &obj)  return true;
            //向下转型
            const auto * tuple = dynamic_cast<const AbstractTuple*>(&obj);
            if (tuple == null) return false; //类型不匹配
            return elements[0] == tuple->elements[0] && elements[1] == tuple->elements[1] && elements[2] == tuple->elements[2];
        }

        //通过下标访问elements数组，省略下标检查
        T operator[](size_t index) const {
            return elements[index];
        }

        T & operator[](size_t index) {
            return elements[index];
        }
    };
}

#endif //RENDERERTEST_ABSTRACTTUPLE_HPP
