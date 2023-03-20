#include "histograms.h"
#include <vector>
#include <iostream>


Histograms::Image::Image(const std::string name) : name(name) {
//    this->name = name;
    this->image = cv::imread(this->name, cv::IMREAD_ANYDEPTH);
}

Histograms::Image::Image(const Image &other) {
    this->height = other.height;
    this->width = other.width;
    this->mag = other.mag;
    this->flipped = other.flipped;
    this->angles = other.angles;
    this->image = other.image.clone();

    this->counter++;
}

Histograms::Image &Histograms::Image::operator=(const Image &other) {
    this->ClearAll();

    this->height = other.height;
    this->width = other.width;
    this->mag = other.mag;
    this->flipped = other.flipped;
    this->angles = other.angles;
    this->image = other.image.clone();

    this->counter++;
    return *this;
}

Histograms::Image::~Image() {
    this->ClearAll();
}

void Histograms::Image::MagAngles() {
    cv::Mat gx, gy, mag, angles;
    cv::Sobel(this->image, gx,  CV_32F, 1, 0, 1);
    cv::Sobel(this->image, gy,  CV_32F, 0, 1, 1);
    cv::cartToPolar(gx, gy, mag, angles, 1);

    this->mag = MatToArray(mag);
    this->angles = MatToArray(angles);
}

void Histograms::Image::Transposition() {
    cv::transpose(this->image, this->image);
    this->height = this->image.rows;
    this->width = this->image.cols;
}

void Histograms::Image::Flip() {
    cv::flip(this->image, this->image, 1);
    if (this->flipped == 0) {
        this->flipped += 1;
    } else {
        this->flipped = 0;
    }
}

void Histograms::Image::CorrectImage() {
    this->Transposition();
    this->MagAngles();
    this->height -= 2;
    this->width -= 2;
}

void Histograms::Image::WriteImage(char* name, float angle) {
    cv::transpose(this->image, this->image);
    if (this->flipped == 1) {
        this->Flip();
    }
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(cv::Point2f(this->width / 2, this->height / 2), angle / 2, 1.0);
    cv::Mat fixed_matrix_first;
    cv::warpAffine(this->image, fixed_matrix_first, rotation_matrix, this->image.size(), cv::INTER_CUBIC);
    cv::imwrite(name, fixed_matrix_first);
}

float** Histograms::Image::MatToArray(cv::Mat &mat) {
    float **array = new float*[mat.rows-2];
    for (int i=0; i<mat.rows-2; ++i)
        array[i] = new float[mat.cols-2];

    for (int i=1; i<mat.rows-1; ++i)
        for (size_t j = 1; j < mat.cols-1; j++)
            array[i-1][j-1] = mat.at<float>(i, j);

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
        ClearMemory(mag);
        ClearMemory(angles);
    }
    this->counter--;
}
