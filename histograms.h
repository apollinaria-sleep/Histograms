#ifndef ___HISTOGRAMS_H
#define ___HISTOGRAMS_H

#include "string"
#include "vector"
#include <exception>
#include <string>
#include <string_view>
#include <deque>
#include "stdlib.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

    /*!
        \brief Класс Histograms позволяет находить угол отклонения оси по двум изображениям и корректировать его.
        \details Каждый объект класса Histograms хранит в себе следующую информацию:
        * diff_angle - угол отклонения оси, найденный по алгоритму
        * part_threshold - доля пикселей изображения, которую отсечет алгоритм
        * threshold - значение, по которому происходит отсечение
        * part_bound - доля пикселей изображения, которые попадут в рамку
        * bound - размер рамки
        * first - первое обработанное изображение
        * second - второе обработанное изображение
        * index_left - индекс середины первой рамки
        * index_right - индекс середины второй рамки
        * first_image - углы изменения интенсивности первого изображения в рамке с учетом threshold
        * second_image - углы изменения интенсивности второго изображения в рамке с учетом threshold
        * first_hist - гистограмма по напрвлениям для первого изображения
        * second_hist - гистограмма по нраправлениям для второго изображения
    */
class Histograms {
private:
    /*!
        \brief Класс Image реализует обработку изображений типа .tif.
        \details Каждый объект класса Image хранит в себе следующую информацию:
        * name - путь к исходному изображению
        * height - высота изображения после преобразований внутри Image
        * width - ширина изображения после преобразований внутри Image
        * counter - счетчик ссылок на объект
        * image - изображение
        * grad_x - указатель на массив размера (height, width) с градиентами по оси х
        * grad_y - указатель на массив размера (height, width) с градиентами по оси y
        * mod - указатель на массив размера (height, width) c sqrt(grad_x^2 + grad_y^2)
    */
    class Image {
    public:
        const char* name;
        size_t height;
        size_t width;
        size_t counter = 1;
        cv::Mat image;
        float** grad_x = nullptr;
        float** grad_y = nullptr;
        float** mod = nullptr;

        Image() = default;
        /*!
            * Создает объект класса Image
            * @param name путь к изображению
            * @param flip - требуется ли отзеркалить изображение
        */
        Image(const char* name);
        Image(const Image &other);
        Image &operator=(const Image &other);
        ~Image();

        /*!
            * Последовательно рассчитывает градиенты изображения, mod, транспонирует изображение, если это требуется
        */
        void CorrectImage();
        /*!
            * Сохраняет новое изображение по указанному пути. При чем новое изображение - это повернутое на угол angle начальное
            * @param name - путь, по которому необходимо сохранить новое изображение
            * @param angle - угол, на который будет повернуто изображение
            * @note Если угол не будет указан, то сохранится начальное изображение
        */
        void WriteImage(char* name, float angle = 0);

    private:
        /*!
            * Ищет максимальное по модулю значение в массиве по указателю
            * @param array - указатель на массив размера (height, width)
            * @return Максимальное значение в массиве по указателю
            * @note Массив по указателю обязательно должен совпадать по размеру с изображением
        */
        float MaxInArray(float** array);
        /*!
            * Транспонирует массив по указателю
            * @param array - указатель на массив размера (height, width)
            * @return Указатель на транспонированный массив
            * @note Массив по указателю обязательно должен совпадать по размеру с изображением
            * @note Начальный массив будет очищен и указатель будет недействителен
        */
        float** TransposeArray(float** array);
        /*!
            * Переводит объект cv::Mat из OpenCV в массив
            * @param mat - объект cv::Mat, который необходимо перевести в массив
            * @return Указатель на массив, совпадающий по смыслу с mat
        */
        float** MatToArray(cv::Mat& mat);
        /*!
            * Очищает память по указанному адресу
            * @note Массив по указанному адресу должен совпадать по высоте с изображением
        */
        void ClearMemory(float** array);
        /*!
            * Очищает всю память выделенную классом (grad_x, grad_y, mod)
        */
        void ClearAll();

        /*!
            * Рассчитывает градиенты изображения по осям x и y c помощью оператора Собеля
            * @note Использован cv::Sobel из OpenCV
        */
        void Gradients();
        /*!
            * Для каждого пикселя рассчитывает sqrt(grad_x^2 + grad_y^2)
        */
        void Mod();
        /*!
            * Транспонирует все изображение, то есть транспонирует каждую состовляющую класса кроме image
        */
        void Transposition();
    };

    float diff_angle = -365;
    float part_threshold = 0.20;
    float part_bound = 0.30;
    float threshold;
    int bound;
    Image first;
    Image second;
    size_t index_left;
    size_t index_right;
    std::vector<float> first_image;
    std::vector<float> second_image;
    std::deque<float> first_hist;
    std::deque<float> second_hist;

public:
    Histograms() = default;
    /*!
        * Создает объект класса Histograms
        * @param image_name_0 - путь к первому изображению
        * @param image_name_1 - путь ко второму изображению
        * @param flip - требуется ли отзеркалить изображение
        * @note Изображения должны иметь одинаковый размер
    */
    Histograms(const char* image_name_0, const char* image_name_1);
    ~Histograms() = default;

    /*!
        * Позволяет изменить долю пикселей изображения, которую отсечет алгоритм
        * @param part_threshold - новая доля
        * @note 0 < part_threshold < 1
    */
    void SetPartThreshold(float part_threshold);
    /*!
        * Позволяет изменить долю пикселей изображения, которые попадут в рамку
        * @param part_bound - новая доля
        * @note 0 < part_bound < 1
    */
    void SetPartBound(float part_bound);
    /*!
        * Позволяет узнать долю пикселей изображения, которую отсечет алгоритм
        * @return Текущая доля
    */
    float CheckPartThreshold();
    /*!
        * Позволяет изменить долю пикселей изображения, которые попадут в рамку
        * @return Текущая доля
    */
    float CheckPartBound();

    /*!
        * Вычисляет, если требуется, и показывает угол смещения оси
        * @param accuracy - точность, с которой требуется найти угол
        * @return найденный угол смещения оси
    */
    float FindAngle(float accuracy = 0.5);
    /*!
        * Сохраняет по заданному пути скорректированные изображения. Также позволяет самостоятельно задать угол корректировки
        * @param first_name - путь, по которому надо сохранить первое скорректированное изображение
        * @param second_name - путь, по которому надо сохранить второе скорректированное изображение
        * @param angle - угол, на который необходимо выполнить поворот
        * @note angle - необязательный параметр, если его не указать, то изображения будут скорректированы по углы, найденному алгоритмом
    */
    void CorrectImages(char* first_name, char* second_name, float angle = -365);

private:
    class Exceptions : public std::exception {
    public:
        explicit Exceptions(std::string_view error) : m_error{error} {}
        const char* what() const noexcept override {
            return m_error.c_str();//;
        }
    private:
        std::string m_error;
    };

    /*!
        * Находит threshold по первому изображению
        * @return threshold
    */
    void Threshold();
    /*!
        * Находит bound по первому изображению
        * @return bound
    */
    void Bound();
    /*!
        * Находит index_left, index_right
    */
    void SearchImportantPart();
    /*!
        * Выделяет наиболее важную часть изображения
    */
    void CutImportantPart();
    /*!
        * Строит гистограмму по наиболее важной части изображения
        * @param array - массив, содержащий наиболее важную часть изображения
        * @param hist - адрес, по которому будет сохранена гистограмма
        * @param n_buck - количество бакетов, на которые будет разбита гистограмма
        * @note Минимальное количество бакетов - 360
    */
    void BuildHistogram(std::vector<float>& array, std::deque<float>& hist, int n_buck = 360);
    /*!
        * Сравнивает гистограммы
        * @param first_hist - первая гистограмма
        * @param second_hist - вторая гистограмма
        * @return Разница между гистограммами
    */
    float CompareHistograms(std::deque<float>& first_hist, std::deque<float>& second_hist);
    /*!
        * Ищет угол смещения оси с заданной точностью
        * @param accuracy - точность, с которой необходимо найти угол
    */
    void SearchAngle(float accuracy);
};

#endif //___HISTOGRAMS_H
// сложный тест
// документация
// инструкция
