#include <iostream>
#include <string>
#include <iomanip>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>

using namespace std;
using namespace cv;


int main();


int Clamp(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

Mat chooseimg()
{
    cout << endl << "Enter full name of the file: " << endl;
    string imagename = "";
    cin >> imagename;

    try
    {
        Mat img = imread(samples::findFile(imagename), IMREAD_COLOR);
        if (img.empty())
        {
            cout << "Image cannot be loaded..!!" << endl;
            exit(0);
        }

        return img;
    }
    catch (cv::Exception)
    {
        cout << "Image cannot be loaded..!!" << endl;
        exit(0);
    }


}


int task_1()
{
    Mat image = chooseimg(), newImage;
    double Mean = 0.0, StdDev = 10.0;
    randn(newImage, Scalar::all(Mean), Scalar::all(StdDev));  // filling with random pixel nums

    for (int Rows = 0; Rows < image.rows; Rows++)
    {
        for (int Cols = 0; Cols < image.cols; Cols++)
        {
            Vec3b Source_Pixel = image.at<Vec3b>(Rows, Cols);
            Vec3s Noise_Pixel = newImage.at<Vec3s>(Rows, Cols);

            for (int i = 0; i < 3; i++)
            {
                int Dest_Pixel = Source_Pixel.val[i] + Noise_Pixel.val[i];
                Noise_Pixel.val[i] = Clamp(Dest_Pixel,0, 255);
            }
        }
    }

    namedWindow("original_image", WINDOW_AUTOSIZE);
    imshow("original_image", image);

    namedWindow("result_image", WINDOW_AUTOSIZE);
    imshow("result_image", newImage);

    waitKey(0);
    return 0;
    main();
}


int task_2a()
{
    float kernel[3][3];
    kernel[0][0] = 1.f / 9.f;
    kernel[1][0] = 1.f / 9.f;
    kernel[2][0] = 1.f / 9.f;

    kernel[1][0] = 1.f / 9.f;
    kernel[1][1] = 1.f / 9.f;
    kernel[1][1] = 1.f / 9.f;

    kernel[2][0] = 1.f / 9.f;
    kernel[2][1] = 1.f / 9.f;
    kernel[2][2] = 1.f / 9.f;

    Mat img = chooseimg();
    Mat NewImage = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat kernel_matrix = Mat(3, 3, CV_32FC1, kernel);
    namedWindow("Reference window", WINDOW_AUTOSIZE);
    imshow("Reference window", img);

    for (int x = 0; x < img.cols; x++)
        for (int y = 0; y < img.rows; y++)
        {
            int radiusX = kernel_matrix.cols / 2;
            int radiusY = kernel_matrix.rows / 2;

            float resultR = 0;
            float resultG = 0;
            float resultB = 0;
            for (int l = -radiusX; l <= radiusX; l++)
                for (int k = -radiusY; k <= radiusY; k++)
                {
                    int idX = Clamp(x + l, 0, img.cols - 1);
                    int idY = Clamp(y + k, 0, img.rows - 1);
                    Vec3b color1 = img.at<Vec3b>(Point(idX, idY));
                    Vec3b color2 = color1;
                    resultR += color1.val[0] * kernel[k + radiusX][l + radiusY];
                    resultG += color1.val[1] * kernel[k + radiusX][l + radiusY];
                    resultB += color1.val[2] * kernel[k + radiusX][l + radiusY];


                    color2.val[0] = Clamp((int)resultR, 0, 255);
                    color2.val[1] = Clamp((int)resultG, 0, 255);
                    color2.val[2] = Clamp((int)resultB, 0, 255);

                    NewImage.at<Vec3b>(Point(x, y)) = color2;
                }
        }


    namedWindow("user converted image", WINDOW_AUTOSIZE);
    imshow("user converted image", NewImage);


    waitKey(0);
    destroyAllWindows();
    main();
}



int task_2a_var2()
{

    Mat img = chooseimg();
    Mat dst = img.clone();

    namedWindow("original", WINDOW_AUTOSIZE);
    namedWindow("cvFilter2D", WINDOW_AUTOSIZE);

    float kernel[9];
    kernel[0] = 1.f / 9.f;
    kernel[1] = 1.f / 9.f;
    kernel[2] = 1.f / 9.f;

    kernel[3] = 1.f / 9.f;
    kernel[4] = 1.f / 9.f;
    kernel[5] = 1.f / 9.f;

    kernel[6] = 1.f / 9.f;
    kernel[7] = 1.f / 9.f;
    kernel[8] = 1.f / 9.f;

    Mat kernel_matrix = Mat(3, 3, CV_32FC1, kernel);
    blur(img, dst, Size(3, 3), Point(-1, -1));

    imshow("original", img);
    imshow("cvFilter2D", dst);

    waitKey(0);
    destroyAllWindows();
    main();
}




int task_2b(int radius, float sigma)
{

    int size = 2 * radius + 1;

    float** kernel;
    kernel = new float* [size];
    for (int i = 0; i < size; i++) {
        kernel[i] = new float[size];
    }

    float norm = 0;

    for (int i = -radius; i <= radius; i++)
        for (int j = -radius; j <= radius; j++)
        {
            kernel[i + radius][j + radius] = (float)(exp(-(i * j + j * j) / (sigma * sigma)));
            norm += kernel[i + radius][j + radius];
        }

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            kernel[i][j] /= norm;

    Mat img = chooseimg();
    Mat NewImage = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat kernel_matrix = Mat(3, 3, CV_32FC1, kernel);
    namedWindow("Reference window", WINDOW_AUTOSIZE);
    imshow("Reference window", img);

    int radiusX = kernel_matrix.cols / 2;
    int radiusY = kernel_matrix.rows / 2;

    for (int i = 0; i < img.cols; i++)
        for (int j = 0; j < img.rows; j++)
        {
            float resultR = 0;
            float resultG = 0;
            float resultB = 0;
            for (int k = -radiusX; k <= radiusX; k++)
                for (int l = -radiusY; l <= radiusY; l++)
                {
                    int idX = Clamp(i + k, 0, img.cols - 1);
                    int idY = Clamp(j + l, 0, img.rows - 1);
                    Vec3b color1 = img.at<Vec3b>(Point(idX, idY));
                    Vec3b color2 = color1;
                    resultR += color1.val[0] * kernel[k + radiusX][l + radiusY];
                    resultG += color1.val[1] * kernel[k + radiusX][l + radiusY];
                    resultB += color1.val[2] * kernel[k + radiusX][l + radiusY];


                    color2.val[0] = Clamp((int)resultR, 0, 255);
                    color2.val[1] = Clamp((int)resultG, 0, 255);
                    color2.val[2] = Clamp((int)resultB, 0, 255);

                    NewImage.at<Vec3b>(Point(i, j)) = color2;
                }
        }


    namedWindow("user converted image", WINDOW_AUTOSIZE);
    imshow("user converted image", NewImage);


    waitKey(0);
    destroyAllWindows();
    main();
}

int task_2b_var2(int radius, float sigma)
{
    Mat img = chooseimg();
    Mat dst = img.clone();

    namedWindow("original", WINDOW_AUTOSIZE);
    namedWindow("cvFilter2D", WINDOW_AUTOSIZE);

    int size = 2 * radius + 1;

    float** kernel;
    kernel = new float* [size];
    for (int i = 0; i < size; i++) {
        kernel[i] = new float[size];
    }

    float norm = 0;

    for (int i = -radius; i <= radius; i++)
        for (int j = -radius; j <= radius; j++)
        {
            kernel[i + radius][j + radius] = (float)(exp(-(i * j + j * j) / (sigma * sigma)));
            norm += kernel[i + radius][j + radius];
        }

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            kernel[i][j] /= norm;

    Mat kernel_matrix = Mat(size, size, CV_32FC1, kernel);
    blur(img, dst, Size(3, 3), Point(-1, -1));

    imshow("original", img);
    imshow("cvFilter2D", dst);

    waitKey(0);
    destroyAllWindows();
    main();

}

int task_3()
{
    cout << "Performing a noise removal using OpenCV fastNIMeansDenoisingColored() method: " << endl;

    int a[4] = { 40, 40, 7, 21 };

    Mat image = chooseimg(), newImage;
    fastNlMeansDenoisingColored(image, newImage, a[0], a[1], a[2], a[3]);

    namedWindow("original_image", WINDOW_AUTOSIZE);
    imshow("original_image", image);

    namedWindow("result_image", WINDOW_AUTOSIZE);
    imshow("result_image", newImage);

    waitKey(0);
    return 0;
    main();
}

int task_3_var2() 
{
    int MAX_KERNEL_LENGTH = 31;
    Mat image = chooseimg(), newImage;

    for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
    {
        bilateralFilter(image, newImage, i, i * 2, i / 2);
    }

    namedWindow("original_image", WINDOW_AUTOSIZE);
    imshow("original_image", image);

    namedWindow("result_image", WINDOW_AUTOSIZE);
    imshow("result_image", newImage);

    waitKey(0);
    return 0;
    main();
}

int main()
{
    int opt;

    cout
        << endl
        << "   Main Menu    " << endl
        << endl << "Write 1 for task_1" << endl
        << endl << "Write 2 for task_2a" << endl
        << endl << "Write 3 for task_2a_var2" << endl
        << endl << "Write 4 for task_2b" << endl
        << endl << "Write 5 for task_2b_var2" << endl
        << endl << "Write 6 for task_3" << endl
        << endl << "Write 6 for task_3_var2" << endl
        << endl
        << endl << "Write 0 to exit" << endl
        << endl
        ;

    cin >> opt;
    cout << "\n" << endl;

    switch (opt)
    {
    case 1:
        task_1();
        break;
    case 2:
        task_2a();
        break;
    case 3:
        task_2a_var2();
        break;
    case 4:
        task_2b(2, 1);
        break;
    case 5:
        task_2b_var2(2, 1);
        break;
    case 6:
        task_3();
        break;
    case 7:
        task_3_var2();
        break;
    case 0:
        exit(1);
    default:
        break;
    }


    return 0;
}