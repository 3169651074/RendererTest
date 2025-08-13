#ifndef RENDERERTEST_MIXTUREPDF_HPP
#define RENDERERTEST_MIXTUREPDF_HPP

#include <util/AbstractPDF.hpp>

namespace renderer {
    /*
     * 混合PDF，使用平衡的启发式权重
     */
    class MixturePDF : public AbstractPDF {
    private:
        std::vector<std::shared_ptr<AbstractPDF>> pdfList;

    public:
        explicit MixturePDF(const std::vector<std::shared_ptr<AbstractPDF>> & pdfList) : pdfList(pdfList) {}

        ~MixturePDF() override = default;

        Vec3 generate() const override {
            //从PDF列表中随机选择一个
            const int index = randomInt(0, static_cast<int>(pdfList.size()) - 1);
            return pdfList[index]->generate();
        }

        double value(const Vec3 &vec) const override {
            //求所有PDF的平均值
            const size_t size = pdfList.size();
            const double weight = 1.0 / static_cast<int>(size);

            double sum = 0.0;
            for (size_t i = 0; i < size; i++) {
                sum += weight * pdfList[i]->value(vec);
            }
            return sum;
        }

        bool equals(const AbstractObject &obj) const override {
            if (this == &obj) return true;
            const auto * pdf = dynamic_cast<const MixturePDF *>(&obj);
            if (pdf == null) return false;
            return pdfList == pdf->pdfList;
        }

        std::string toString() const override {
            std::string ret("Mixture PDF: Size = ");
            ret += std::to_string(pdfList.size());
            for (size_t i = 0; i < pdfList.size(); i++) {
                ret += "\n\t[" + std::to_string(i) + std::string("] = ") + pdfList[i]->toString();
            }
            return ret;
        }
    };
}

#endif //RENDERERTEST_MIXTUREPDF_HPP
