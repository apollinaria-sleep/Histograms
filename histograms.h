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
#include <fstream>

    /*!
        \brief Класс Histograms позволяет находить угол отклонения оси по двум изображениям и корректировать его.
        \details Каждый объект класса Histograms хранит в себе следующую информацию:
        * diff_angle - угол отклонения оси, найденный по алгоритму
        * part_threshold - доля пикселей изображения, которую отсечет алгоритм
        * threshold - значения, по которому происходит отсечение для каждого изображения
        * part_bound - доля пикселей изображения, которые попадут в рамку
        * bound - размер рамки
        * first - первое обработанное изображение
        * second - второе обработанное изображение
        * indexes - индексы, которые показывают положение середины рамки для каждого изображения
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
        * height - высота изображения после преобразований внутри Image
        * width - ширина изображения после преобразований внутри Image
        * mag - указатель на массив размера (height, width) c sqrt(grad_x^2 + grad_y^2)
        * angles - указатель на массив с углами atan2(grad_x, grad_y)
    */
    class Image {
    public:
        size_t height;
        size_t width;
        float** mag = nullptr;
        float** angles = nullptr;

        Image() = default;
        /*!
            * Создает объект класса Image
            * @param name путь к изображению
        */
        Image(const std::string name);
        Image(const Image &other);
        Image &operator=(const Image &other);
        ~Image();

        /*!
            * Переворачивает изображение вдоль оси У
        */
        void Flip();
        /*!
            * Последовательно транспонирует изображение, рассчитывает градиенты изображения c помощью оператора Собеля, углы и модули
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
        const std::string name;
        size_t counter = 1;
        cv::Mat image;
        int flipped = 0;

        void MagAngles();
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
            * Очищает всю память выделенную классом (mag, angles)
        */
        void ClearAll();

        /*!
            * Транспонирует изображение
        */
        void Transposition();
    };

    float diff_angle = -365;
    float part_threshold = 0.2;
    float part_bound = 0.30;
    float threshold[2] {0, 0};
    int bound;
    Image first;
    Image second;
    size_t indexes[4] {0,0,0,0};
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
        * @note Изображения должны иметь одинаковый размер
    */
    Histograms(const std::string image_name_0, const std::string image_name_1);
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
        * Вычисляет, если требуется, и показывает угол смещения оси с точностью до 0.5 градуса
        * @return найденный угол смещения оси
    */
    float FindAngle();
    /*!
        * Сохраняет по заданному пути скорректированные изображения. Также позволяет самостоятельно задать угол корректировки
        * @param first_name - путь, по которому надо сохранить первое скорректированное изображение
        * @param second_name - путь, по которому надо сохранить второе скорректированное изображение
        * @param angle - угол, на который необходимо выполнить поворот
        * @note angle - необязательный параметр, если его не указать, то изображения будут скорректированы по углы, найденному алгоритмом
    */
    void CorrectImages(char* first_name, char* second_name, float angle = -365);
    /*!
        * Создает файл с заданным именем, который затем можно визуализировать с помощью файла Отрисовка гистограммы.ipynb
        * @param name - путь к файлу, в который надо будет сохранить гистограммы
    */
    void VisualizeHistograms(char* name);

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
    float Threshold(float** mag);
    /*!
        * Находит bound по первому изображению
        * @return bound
    */
    void Bound();
    /*!
        * Находит index_left, index_right
    */
    void SearchImportantPart(float** mag, size_t index_left, size_t index_right);
    /*!
        * Выделяет наиболее важную часть изображения
    */
    void CutImportantPart(float** mag, float** angles, size_t index_left, size_t index_right, std::vector<float>& image);
    /*!
        * Строит гистограмму по наиболее важной части изображения
        * @param array - массив, содержащий наиболее важную часть изображения
        * @param hist - адрес, по которому будет сохранена гистограмма
        * @param n_buck - количество бакетов, на которые будет разбита гистограмма
        * @note Минимальное количество бакетов - 360
    */
    void BuildHistogram(std::vector<float>& array, std::deque<float>& hist);
    /*!
        * Сравнивает гистограммы
        * @param first_hist - первая гистограмма
        * @param second_hist - вторая гистограмма
        * @return Разница между гистограммами
    */
    float CompareHistograms(std::deque<float>& first_hist, std::deque<float>& second_hist);
    /*!
        * Ищет угол смещения оси с заданной точностью
    */
    void SearchAngle();
};

#endif //___HISTOGRAMS_H
