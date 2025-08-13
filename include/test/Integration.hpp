#include <Global.hpp>
#include <fstream>
using namespace std;

namespace renderer {
    class IntegrationTest {
    public:
        static void estimatePI() {
            /*
             * 使用随机采样法估算π：在正方形内随机取点，计算落在内切于正方形的圆之内的点的个数
             * π / 4 = 圆内点数 / 总点数
             * 随N值增大，估计值逐渐收敛于π
             */
            const auto N = static_cast<size_t>(1e6);
            const double RADIUS = 1.0;
            size_t insideCircle = 0;

            for (size_t i = 0; i < N; i++) {
                //在正方形内随机取点，取圆半径为1
                const double x = randomDouble(-RADIUS, RADIUS);
                const double y = randomDouble(-RADIUS, RADIUS);

                if (x * x + y * y <= RADIUS * RADIUS) {
                    insideCircle++;
                }
            }

            const double estimate = 4.0 * insideCircle / N;
            cout << fixed << setprecision(12) << estimate << endl;
        }

        /*
         * 使用蒙特卡洛方法求积分的近似值
         * 函数指针f指向原函数
         */
        static void integrate(double (*func)(double), double a, double b) {
            constexpr size_t N = 1000000;
            double sum = 0.0;

            for (size_t i = 0; i < N; i++) {
                //在区间内随机取点
                const double xi = randomDouble(a, b);
                //累加
                sum += (*func)(xi);
            }

            //取平均值
            const double result = (b - a) * (sum / N);
            cout << fixed << setprecision(12) << result << endl;
        }

        /*
         * 将积分值等分为两份，求等分点处的x值
         */
        static void divideIntegrationArea(double (*func)(double), double a, double b) {
            constexpr size_t N = 1000000;
            double sum = 0.0;

            //临时存储采样点和其函数值
            //此处不能使用局部数组，由于N的值很大，栈空间不足造成内存违规访问，在函数调用时报错退出
            typedef pair<double, double> Sample;
            vector<Sample> samples(N);

            for (size_t i = 0; i < N; i++) {
                const double xi = randomDouble(a, b);
                const double val = (*func)(xi);
                sum += val;

                //在积分时记录采样信息
                samples[i].first = xi;
                samples[i].second = val;
            }

            //对采样信息数组进行排序
            sort(samples.begin(), samples.end(),
                 [](const Sample & p1, const Sample & p2)
            {
                     //按采样信息的x值进行排序
                     return p1.first < p2.first;
            });

            //按照排序后的采样信息数组，从小到大累加val值，直到和超过sum的一半
            const double halfSum = sum / 2.0;
            double middlePoint = 0.0;
            double iterateSum = 0.0;

            for (size_t i = 0; i < N; i++) {
                iterateSum += samples[i].second;
                if (iterateSum >= halfSum) {
                    middlePoint = samples[i].first;
                    break;
                }
            }

            cout << fixed << setprecision(12);
            cout << "sum = " << sum << ", half sum = " << halfSum << "\n";
            cout << "iterate sum = " << iterateSum << "\n";
            cout << "middle point = " << middlePoint << endl;
        }

        /*
         * 使用概率密度函数和分布函数的反函数（ICD）估算区间积分
         * 积分区间包含在pdf函数和icd函数中。icd(0) = 0, icd(1) = 2，则积分区间为[0, 2]
         *
         * 分布函数将积分区间的下限 a 映射到0，将上限 b 映射到1（分布函数的性质）
         * 其反函数则相反，将 [0, 1] 区间的值映射回积分区间 [a, b]
         * ICD函数的输入值域为[0, 1]，输出值域就是隐含的积分区间[a, b]
         */
        static void distributionIntegrate(double (*func)(double), double (funcPDF)(double), double (*funcICD)(double)) {
            constexpr size_t N = 1;
            double sum = 0.0;

            for (size_t i = 0; i < N; i++) {
                const double ran = randomDouble();
                if (floatValueNearZero(ran)) continue;

                const double x = (*funcICD)(ran);
                const double val = (*func)(x) / (*funcPDF)(x);
                sum += val;
            }

            const double result = sum / N;
            cout << fixed << setprecision(12) << "I = " << result << endl;
        }

        static double f(double x) {
            return x * x;
        }

        static double icd(double x) {
            //return x * 2.0;
            //return std::sqrt(4.0 * x);
            return 2.0 * std::pow(x, 1.0 / 3.0);
        }

        static double pdf(double x) {
            //return 0.5;
            //return x / 2.0;
            return 3.0 / 8.0 * x * x;
        }

        /*
         * 将两个均匀分布的随机数（在球面上的极坐标）转换为直角坐标，从而在空间上显示
         */
        static void randomPointsOnUnitSphere() {
            constexpr size_t N = 100;
            ofstream os("../files/coordinate.txt");
            if (!os.is_open()) {
                cout << "Failed to open file!" << endl;
                return;
            }

            for (size_t i = 0; i < N; i++) {
                const auto r1 = randomDouble();
                const auto r2 = randomDouble();

                const auto x = cos(2.0 * PI * r1) * 2.0 * sqrt(r2 * (1.0 - r2));
                const auto y = sin(2.0 * PI * r1) * 2.0 * sqrt(r2 * (1.0 - r2));
                const auto z = 1.0 - 2.0 * r2;
                cout << x << " " << y << " " << z << endl;
                os << x << " " << y << " " << z << endl;
            }
            os.close();
        }

        /*
         * 求函数在半球面（极坐标范围theta -> [0, 2π], phi -> [0, 0.5π]）的二重积分
         * 由于积分区域是一个面而不是体，因此为二重而不是三重积分
         */
        static void hemisphereIntegrate(double (*func)(double), double (*funcPDF)(double)) {
            constexpr size_t N = 1000000;
            double sum = 0.0;

            for (size_t i = 0; i < N; i++) {
                const double xi = randomDouble();
                const double val = (*func)(xi) / (*funcPDF)(xi);
                sum += val;
            }

            const double result = sum / N;
            cout << fixed << setprecision(12) << "Result: " << result << "\n";
            //余弦函数的立方在半球上的积分值为0.5π
            cout << "PI / 2: " << (PI / 2.0) << endl;
        }

        static double f2(double x) {
            return pow(1.0 - x, 3.0);
        }

        static double pdf2(double x) {
            return 1.0 / (2.0 * PI);
        }

        /*
         * 修改p(x) = cos(theta) / π，其中theta为从原点出发指向(x, y, z)的向量和z轴正半轴的夹角
         * 此pdf使得theta小的方向，即接近z正半轴的方向，生成的向量更多
         * 生成的向量非单位向量
         *
         * axis表示向量聚集的轴，传入2等同于以上的z轴
         * toPositive表示聚集的方向是否为正半轴
         */
        static void randomCosinePointsOnUnitSphere(int axis = 2, bool toPositive = true) {
            constexpr size_t N = 100;
            ofstream os("../files/coordinate.txt");
            if (!os.is_open()) {
                cout << "Failed to open file!" << endl;
                return;
            }

            double coord[3];
            for (size_t i = 0; i < N; i++) {
                const auto r1 = randomDouble();
                const auto r2 = randomDouble();

                coord[0] = cos(2.0 * PI * r1) * 2.0 * sqrt(r2);
                coord[1] = sin(2.0 * PI * r1) * 2.0 * sqrt(r2);
                coord[2] = sqrt(1.0 - r2);

                switch (axis) {
                    case 0:
                        swap(coord[0], coord[2]);
                        break;
                    case 1:
                        swap(coord[1], coord[2]);
                        break;
                    case 2:
                    default:
                        break;
                }

                if (!toPositive) {
                    coord[axis] = -coord[axis];
                }

                cout << coord[0] << " " << coord[1] << " " << coord[2] << endl;
                os << coord[0] << " " << coord[1] << " " << coord[2] << endl;
                //cout << sqrt(coord[0] * coord[0] + coord[1] * coord[1] + coord[2] * coord[2]) << endl;
            }
            os.close();
        }
    };
}