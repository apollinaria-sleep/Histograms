#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "histograms.h"

TEST_CASE("Hyperparameters") {
    Histograms test("../Tests/test0_1.tif", "../Tests/test1_1_flip.tif");

    CHECK(test.CheckPartBound() == 0.3f);
    CHECK(test.CheckPartThreshold() == 0.2f);

    test.SetPartBound(0.5f);
    CHECK(test.CheckPartBound() == 0.5f);
    test.SetPartThreshold(0.4f);
    CHECK(test.CheckPartThreshold() == 0.4f);
}

TEST_CASE("EasyTest") {
    Histograms test0("../Tests/test0_1.tif", "../Tests/test0_1_flip.tif");
    CHECK(test0.FindAngle() == 0);

    Histograms test1("../Tests/test0_1.tif", "../Tests/test1_1_flip.tif");
    CHECK(test1.FindAngle() == 1.0f);

    Histograms test2("../Tests/test0_20.tif", "../Tests/test1_20_flip.tif");
    CHECK(test2.FindAngle() == 2.0f);
}

TEST_CASE("Exceptions") {
    bool error = false;

    // Проверка инициализации
    try {
        Histograms test("../Tests/test0_1.tif", "../Tests/test0_20.tif");
    } catch (std::exception& e) {
        error = true;
    }
    CHECK(error == true);

    Histograms test("../Tests/test0_1.tif", "../Tests/test0_1_flip.tif");
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
    const size_t img_cnt = 29;

    const std::string first[img_cnt] {"../proj_src/proj_0001.tif","../proj_src/proj_0004.tif",
                                      "../proj_src/proj_0005.tif","../proj_src/proj_0006.tif",
                                      "../proj_src/proj_0007.tif","../proj_src/proj_0008.tif",
                                      "../proj_src/proj_0009.tif","../proj_src/proj_0010.tif",
                                      "../proj_src/proj_0014.tif","../proj_src/proj_0015.tif",
                                      "../proj_src/proj_0016.tif","../proj_src/proj_0017.tif",
                                      "../proj_src/proj_0018.tif","../proj_src/proj_0019.tif",
                                      "../proj_src/proj_0020.tif", "../proj_src/proj_0021.tif",
                                      "../proj_src/proj_0022.tif","../proj_src/proj_0024.tif",
                                      "../proj_src/proj_0025.tif", "../proj_src/proj_0026.tif",
                                      "../proj_src/proj_0027.tif","../proj_src/proj_0028.tif",
                                      "../proj_src/proj_0030.tif", "../proj_src/proj_0031.tif",
                                      "../proj_src/proj_0032.tif", "../proj_src/proj_0033.tif",
                                      "../proj_src/proj_0035.tif","../proj_src/proj_0036.tif",
                                      "../proj_src/proj_0038.tif"};
    const std::string second[img_cnt] {"../proj_src/proj_0361.tif","../proj_src/proj_0364.tif",
                                       "../proj_src/proj_0365.tif",
                                       "../proj_src/proj_0366.tif", "../proj_src/proj_0367.tif",
                                       "../proj_src/proj_0368.tif", "../proj_src/proj_0369.tif",
                                       "../proj_src/proj_0370.tif","../proj_src/proj_0374.tif",
                                       "../proj_src/proj_0375.tif","../proj_src/proj_0376.tif",
                                       "../proj_src/proj_0377.tif", "../proj_src/proj_0378.tif",
                                       "../proj_src/proj_0379.tif","../proj_src/proj_0380.tif",
                                       "../proj_src/proj_0381.tif", "../proj_src/proj_0382.tif",
                                       "../proj_src/proj_0384.tif","../proj_src/proj_0385.tif",
                                       "../proj_src/proj_0386.tif","../proj_src/proj_0387.tif",
                                       "../proj_src/proj_0388.tif","../proj_src/proj_0390.tif",
                                       "../proj_src/proj_0391.tif","../proj_src/proj_0392.tif",
                                       "../proj_src/proj_0393.tif","../proj_src/proj_0395.tif",
                                       "../proj_src/proj_0396.tif","../proj_src/proj_0398.tif"};

    for (size_t i = 0; i < img_cnt; i++) {
        Histograms test(first[i], second[i]);
        CHECK(test.FindAngle() == 0.5f);
    }
}
