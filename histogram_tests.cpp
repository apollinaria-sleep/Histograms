#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "histograms.h"

TEST_CASE("Hyperparameters") {
    Histograms test("../Tests/test0_25.tif", "../Tests/test1_25.tif");

    CHECK(test.CheckPartBound() == 0.3f);
    CHECK(test.CheckPartThreshold() == 0.2f);

    test.SetPartBound(0.5f);
    CHECK(test.CheckPartBound() == 0.5f);
    test.SetPartThreshold(0.4f);
    CHECK(test.CheckPartThreshold() == 0.4f);
}

TEST_CASE("EasyTest") {
    Histograms test0("../Tests/test0_25.tif", "../Tests/test0_25.tif");
    float accuracy[3] {0.1, 0.25, 0.5};
    for (size_t i = 0; i < 3; i++) {
        CHECK(test0.FindAngle(accuracy[i]) == 0);
    }

    Histograms test("../Tests/test0_25.tif", "../Tests/test1_25.tif");
    CHECK(test.FindAngle(0.25) == 2.5f);
}

TEST_CASE("Exceptions") {
    bool error = false;

    // Проверка инициализации
    try {
        Histograms test("../Tests/test0_125.tif", "../Tests/test1_1.tif");
    } catch (std::exception& e) {
        error = true;
    }
    CHECK(error == true);

    error = false;
    Histograms test("../Tests/test0_25.tif", "../Tests/test1_25.tif");
    try {
        test.FindAngle(0);
    } catch (std::exception& e) {
        error = true;
    }
    CHECK(error == true);

    error = false;
    try {
        test.SetPartThreshold(2);
    } catch (std::exception& e) {
        error = true;
    }
    CHECK(error == true);

    error = false;
    try {
        test.SetPartThreshold(-1);
    } catch (std::exception& e) {
        error = true;
    }
    CHECK(error == true);

    error = false;
    try {
        test.SetPartBound(2);
    } catch (std::exception& e) {
        error = true;
    }
    CHECK(error == true);

    error = false;
    try {
        test.SetPartBound(-1);
    } catch (std::exception& e) {
        error = true;
    }
    CHECK(error == true);
}

TEST_CASE("HardTests") {
#     Histograms test025("../Tests/test0_025.tif", "../Tests/test1_025.tif");
#     CHECK(test025.FindAngle(0.125) == 0.25);

#     Histograms test05("/../Tests/test0_05.tif", "../Tests/test1_05.tif");
#     CHECK(test05.FindAngle(0.25) == 0.5);

    Histograms test125("../Tests/test0_125.tif", "../Tests/test1_125.tif");
    CHECK(test125.FindAngle(0.125) == 1.25);

    Histograms test1("../Tests/test0_1.tif", "../Tests/test1_1.tif");
    CHECK(test1.FindAngle(0.25) == 1.0);
}

TEST_CASE("VeryHardTests") {

}
