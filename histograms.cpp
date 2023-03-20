#include "histograms.h"
#include "tiffio.h"
#include <iostream>
#include <vector>
#include <math.h>


// конструктор Histograms
Histograms::Histograms(const std::string image_name_0, const std::string image_name_1) {
    Image i_0(image_name_0);
    this->first = i_0;
    this->first.CorrectImage();
    Image i_1(image_name_1);
    i_1.Flip();
    this->second = i_1;
    this->second.CorrectImage();

    if (this->first.height != this->second.height || this->first.width != this->second.width) {
        throw Exceptions("Изображения имеют различные размеры\n");
    }
}

float Histograms::FindAngle() {
    if (this->diff_angle == -365) {
        this->SearchAngle();
    }
    return this->diff_angle;
}

void Histograms::CorrectImages(char* first_name, char* second_name, float angle) {
    if (angle == -365) {
        if (this->diff_angle == -365) {
            this->FindAngle();
        }

        this->first.WriteImage(first_name, this->diff_angle / 2);
        this->second.WriteImage(second_name, -this->diff_angle / 2);
    }
}

void Histograms::VisualizeHistograms(char *name) {
    if (this->first_hist.size() == 0 || this->second_hist.size() == 0) {
       this->FindAngle();
    }
    std::ofstream out;          // поток для записи
    out.open(name); // окрываем файл для записи
    if (out.is_open()) {
        out << this->first_hist.size() << '\n';
        for (float& elem : this->first_hist) {
            out << elem << ' ';
        }
        out << '\n';
        for (float& elem : this->second_hist) {
            out << elem << ' ';
        }
    } else {
        throw Exceptions("Не удалось сохранить гистограммы\n");
    }
    out.close();
}

void Histograms::SetPartThreshold(float part_threshold) {
    if (part_threshold <= 0 || part_threshold >= 1) {
        throw Exceptions("0 < Part_threshold < 1");
    }
    this->part_threshold = part_threshold;
}

float Histograms::CheckPartThreshold() {
    return this->part_threshold;
}

float Histograms::CheckPartBound() {
    return this->part_bound;
}

void Histograms::SetPartBound(float part_bound) {
    if (part_bound <= 0 || part_bound >= 1) {
        throw Exceptions("0 < Part_bound < 1");
    }
    this->part_bound = part_bound;
}

float Histograms::Threshold(float** mag) {
    std::vector<float> array(this->first.height * this->first.width);
    for (size_t i = 0; i < this->first.height; i++) {
        for (size_t j = 0; j < this->first.width; j++) {
            array[i * this->first.width + j] = mag[i][j];
        }
    }
    std::sort(array.begin(), array.end());

    return array[int(round(this->first.height * this->first.width * (1 - this->part_threshold)))];
}

void Histograms::Bound() {
    this->bound = int(float(this->first.height) * this->part_bound / 2 / 2);
}

void Histograms::SearchImportantPart(float** mag, size_t index_left, size_t index_right) {
    std::vector<float> sum_array(this->first.height, 0);
    float max_0 = 0;
    float max_1 = 0;
    for (size_t i = 0; i < this->first.height; i++) {
        for (size_t j = fmax(0, i - this->bound); j < fmin(this->first.height, i + this->bound); j++) {
            for (size_t k = 0; k < this->first.width; k++) {
                if (mag[j][k] >= this->threshold[index_left / 2]) {
                    sum_array[i] += mag[j][k];
                }
            }
        }

        if (i <= this->first.height / 2) {
            if (max_0 < sum_array[i]) {
                max_0 = sum_array[i];
                this->indexes[index_left] = i;
            }
        } else {
            if (max_1 < sum_array[i]) {
                max_1 = sum_array[i];
                this->indexes[index_right] = i;
            }
        }
    }
}

void Histograms::CutImportantPart(float** mag, float** angles, size_t index_left, size_t index_right, std::vector<float>& image) {
    for (size_t i = 0; i < this->bound * 4; i++) {
        int index;
        if (i <= this->bound * 2) {
            if (this->indexes[index_left] - this->bound + i < 0) {
                continue;
            }
            index = int(fmax(0, this->indexes[index_left] - this->bound + i));
        } else {
            if (this->indexes[index_right] - this->bound * 3 + i > this->first.height) {
                break;
            }
            index = int(fmin(this->first.height, this->indexes[index_right] - this->bound * 3 + i));
        }
        for (size_t j = 0; j < this->first.width; j++) {
            if (mag[index][j] >= this->threshold[index_left / 2]) {
                image.push_back(angles[index][j]);
            }
        }
    }
}

void Histograms::BuildHistogram(std::vector<float>& array, std::deque<float>& hist) {
    int n_buck = 720;
    float delta = 360.0 / n_buck;

    std::sort(array.begin(), array.end());

    size_t j = 0;
    size_t i = 1;
    while (true) {
        if (j == array.size() || i > n_buck)
            break;
        hist.push_back(0);
        float curr = array[j];
        while (abs(curr - array[j]) <= delta) {
            hist[i - 1] += 1;
            j++;
        }
        hist[i - 1] /= array.size();
        i++;
    }
}

float Histograms::CompareHistograms(std::deque<float>& first_hist, std::deque<float>& second_hist) {
    float difference = 0;
    for (size_t i = 0; i < first_hist.size(); i++) {
        difference += (first_hist[i] - second_hist[i]) * (first_hist[i] - second_hist[i]);
    }

    return difference;
}

void Histograms::SearchAngle() {
    float accuracy = 0.5;

    this->threshold[0] = this->Threshold(this->first.mag);
    this->threshold[1] = this->Threshold(this->second.mag);
    this->Bound();
    this->SearchImportantPart(this->first.mag, 0, 1);
    this->CutImportantPart(this->first.mag, this->first.angles, 0, 1, this->first_image);
    this->SearchImportantPart(this->second.mag, 2, 3);
    this->CutImportantPart(this->second.mag, this->second.angles, 2, 3, this->second_image);
    this->BuildHistogram(this->first_image, this->first_hist);
    this->BuildHistogram(this->second_image, this->second_hist);

    if (this->CompareHistograms(this->first_hist, this->second_hist) == 0) {
        this->diff_angle = 0;
        return;
    }

    float min_diff = 3;
    float t = this->second_hist[this->second_hist.size() - 1];
    this->second_hist.pop_back();
    this->second_hist.push_front(t);
    for (int angle = 1; angle < 20; angle++) {
        float current_diff = this->CompareHistograms(this->first_hist, this->second_hist);
        if (min_diff > current_diff) {
            min_diff = current_diff;
            this->diff_angle = angle * accuracy;
        }
        float t = this->second_hist[this->second_hist.size() - 1];
        this->second_hist.pop_back();
        this->second_hist.push_front(t);
    }

    if (this->diff_angle > 180)
        this->diff_angle -= 360;
}
