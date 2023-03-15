#include "histograms.h"
#include <vector>
#include <iostream>


Histograms::Image::Image(const char *name) : name(name) {
    this->image = cv::imread(this->name, cv::IMREAD_ANYDEPTH);
    cv::rotate(this->image, this->image, cv::ROTATE_90_CLOCKWISE);
    this->height = this->image.rows;
    this->width = this->image.cols;
    std::cout << this->height << ' ' << this->width;
}

Histograms::Image::Image(const Image &other) {
    this->height = other.height;
    this->width = other.width;
    this->grad_x = other.grad_x;
    this->grad_y = other.grad_y;
    this->mod = other.mod;
    this->image = other.image.clone();

    this->counter++;
}

Histograms::Image &Histograms::Image::operator=(const Image &other) {
    this->ClearAll();

    this->height = other.height;
    this->width = other.width;
    this->grad_x = other.grad_x;
    this->grad_y = other.grad_y;
    this->mod = other.mod;
    this->image = other.image.clone();

    this->counter++;
    return *this;
}

Histograms::Image::~Image() {
    this->ClearAll();
}

void Histograms::Image::Gradients() {
    cv::Mat gradients;

    cv::Sobel(this->image, gradients,  CV_32F, 1, 0, 1);
    this->grad_x = MatToArray(gradients);
    cv::Sobel(this->image, gradients,  CV_32F, 0, 1, 1);
    this->grad_y = MatToArray(gradients);
}

void Histograms::Image::Mod() {
    this->mod = new float*[this->height];
    for (size_t i = 0; i < this->height; i++) {
        this->mod[i] = new float[this->width];
        for (size_t j = 0; j < this->width; j++) {
            this->mod[i][j] = std::sqrt(this->grad_x[i][j] * this->grad_x[i][j] + this->grad_y[i][j] * this->grad_y[i][j]);
        }
    }
}

void Histograms::Image::Transposition() {
    this->grad_x = TransposeArray(this->grad_x);
    this->grad_y = TransposeArray(this->grad_y);
    this->mod = TransposeArray(this->mod);
    size_t t = this->height;
    this->height = this->width;
    this->width = t;
}

void Histograms::Image::CorrectImage() {
    this->Gradients();
    this->Mod();
//    if (this->MaxInArray(this->grad_x) > this->MaxInArray(this->grad_y)) {
//    this->Transposition();
//    }
}

void Histograms::Image::WriteImage(char* name, float angle) {
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(cv::Point2f(this->width / 2, this->height / 2), angle / 2, 1.0);
    cv::Mat fixed_matrix_first;
    cv::warpAffine(this->image, fixed_matrix_first, rotation_matrix, this->image.size(), cv::INTER_CUBIC);
    cv::imwrite(name, fixed_matrix_first);
}

float Histograms::Image::MaxInArray(float **array) {
    float max = -1;
    for (size_t i = 0; i < this->height; i++) {
        for (size_t j = 0; j < this->width; j++) {
            if (abs(array[i][j]) > max) {
                max = abs(array[i][j]);
            }
        }
    }
    return max;
}

float** Histograms::Image::TransposeArray(float **array) {
    float** new_array = new float*[this->width];
    for (size_t i = 0; i < this->width; i++) {
        new_array[i] = new float[this->height];
        for (size_t j = 0; j < this->height; j++) {
            new_array[i][j] = array[j][i];
        }
    }
    ClearMemory(array);
    return new_array;
}

float** Histograms::Image::MatToArray(cv::Mat &mat) {
    float **array = new float*[mat.rows];
    for (int i=0; i<mat.rows; ++i)
        array[i] = new float[mat.cols];

    for (int i=0; i<mat.rows; ++i)
        for (size_t j = 0; j < mat.cols; j++)
          array[i][j] = mat.at<float>(i, j);

    return array;
}

void Histograms::Image::ClearMemory(float **array) {
    if (array != nullptr) {
        for (int i = 0; i < this->height; i++) {
            delete[] array[i];
        }
        delete[] array;
    }
}

void Histograms::Image::ClearAll() {
    if (this->counter - 1 == 0) {
        ClearMemory(grad_x);
        ClearMemory(grad_y);
        ClearMemory(mod);
    }
    this->counter--;
}
