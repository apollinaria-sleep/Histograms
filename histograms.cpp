#include "histograms.h"
#include "tiffio.h"
#include <iostream>
#include <vector>
#include <math.h>


// конструктор Histograms
Histograms::Histograms(const char* image_name_0, const char* image_name_1) {
    Image i_0(image_name_0);
    this->first = i_0;
    this->first.CorrectImage();
    Image i_1(image_name_1);
    this->second = i_1;
    this->second.CorrectImage();

    if (this->first.height != this->second.height || this->first.width != this->second.width) {
        throw Exceptions("Изображения имеют различные размеры\n");
    }
}

float Histograms::FindAngle(float accuracy) {
    if (accuracy == 0) {
        throw Exceptions("Недостижимая точность\n");
    }
    if (this->diff_angle == -365) {
        this->SearchAngle(accuracy);
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
    } else {
        while (abs(angle) > 360) {
            if (angle < 0)
                angle += 360;
            else
                angle -= 360;
        }
        this->first.WriteImage(first_name, angle);
        this->second.WriteImage(second_name, angle);
    }
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

void Histograms::Threshold() {
    std::vector<float> array(this->first.height * this->first.width);
    for (size_t i = 0; i < this->first.height; i++) {
        for (size_t j = 0; j < this->first.width; j++) {
            array[i * this->first.width + j] = this->first.mod[i][j];
        }
    }
    std::sort(array.begin(), array.end());
    this->threshold = array[int(this->first.height * this->first.width * (1 - this->part_threshold))];
}

void Histograms::Bound() {
    this->bound = int(float(this->first.height) * this->part_bound / 2 / 2);
}

void Histograms::SearchImportantPart() {
    std::vector<float> sum_array(this->first.height, 0);
    float max_0 = 0;
    float max_1 = 0;
    for (size_t i = 0; i < this->first.height; i++) {
        for (size_t j = fmax(0, i - this->bound); j < fmin(this->first.height, i + this->bound); j++) {
            for (size_t k = 0; k < this->first.width; k++) {
                if (this->first.mod[j][k] >= this->threshold) {
                    sum_array[i] += this->first.mod[j][k];
                }
            }
        }

        if (i <= this->first.height / 2) {
            if (max_0 < sum_array[i]) {
                max_0 = sum_array[i];
                this->index_left = i;
            }
        } else {
            if (max_1 < sum_array[i]) {
                max_1 = sum_array[i];
                this->index_right = i;
            }
        }
    }
}

void Histograms::CutImportantPart() {
    for (size_t i = 0; i < this->bound * 4; i++) {
        for (size_t j = 0; j < this->first.width; j++) {
            int index;
            if (i <= this->bound * 2) {
                index = int(fmax(0, this->index_left - this->bound + i));
            } else {
                index = int(fmin(this->first.height, this->index_right - this->bound * 3 + i));
            }
            if (this->first.mod[index][j] >= this->threshold) {
//                    this->first_image.push_back(this->first.mod[int(fmax(0, this->index_left - this->bound + i))][j]);
                this->first_image.push_back(std::atan2(this->first.grad_y[index][j], this->first.grad_x[index][j]) / M_PI * 180);
            }
//                index = int(fmax(0, this->first.height - this->index_right - this->bound + i)); // для симметричного случая
            if (this->second.mod[index][j] >= this->threshold) {
//                    this->second_image.push_back(this->first.mod[int(fmax(0, this->first.height - this->index_right - this->bound + i))][j]);
                this->second_image.push_back(std::atan2(this->second.grad_y[index][j], this->second.grad_x[index][j]) / M_PI * 180);
            }
        }
    }
}

void Histograms::BuildHistogram(std::vector<float>& array, std::deque<float>& hist, int n_buck) {
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

void Histograms::SearchAngle(float accuracy) {
    if (accuracy > 1) {
        accuracy = 1;
    }

    this->Threshold();
    this->Bound();
    this->SearchImportantPart();
    this->CutImportantPart();
    this->BuildHistogram(this->first_image, this->first_hist, 360 / (accuracy * 2));
    this->BuildHistogram(this->second_image, this->second_hist, 360 / (accuracy * 2));

    if (this->CompareHistograms(this->first_hist, this->second_hist) == 0) {
        this->diff_angle = 0;
        return;
    }

    float min_diff = 3;
    float t = this->second_hist[this->second_hist.size() - 1];
    this->second_hist.pop_back();
    this->second_hist.push_front(t);
//    float t = this->second_hist[0];
//    this->second_hist.pop_front();
//    this->second_hist.push_back(t);
    std::cout << accuracy * 2;
////    for (int angle = 1; angle < 360 / (accuracy * 2); angle++) {
    for (int angle = 1; angle < 5 / (accuracy * 2); angle++) {
        float current_diff = this->CompareHistograms(this->first_hist, this->second_hist);
//        std::cout << current_diff << ' ' << angle << ' ' << this->diff_angle << '\n';
        if (min_diff > current_diff) {
            min_diff = current_diff;
            this->diff_angle = angle * (accuracy * 2);
        }
        float t = this->second_hist[this->second_hist.size() - 1];
        this->second_hist.pop_back();
        this->second_hist.push_front(t);
//        float t = this->second_hist[0];
//        this->second_hist.pop_front();
//        this->second_hist.push_back(t);
    }

    if (this->diff_angle > 180)
        this->diff_angle -= 360;
}