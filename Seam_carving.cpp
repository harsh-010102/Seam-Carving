#include <opencv4/opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int **energy_cal(int ***pixel, int rows, int cols)
{
    int **energy_cal2 = new int *[rows];
    for (int i = 0; i < rows; i++)
    {
        energy_cal2[i] = new int[cols];
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int x, y, r, g, b;
            int m = (i - 1 + rows) % rows;
            int n = (i + 1) % rows;
            {
                r = (pixel[m][j][0] - pixel[n][j][0]) * (pixel[m][j][0] - pixel[n][j][0]);
                b = (pixel[m][j][1] - pixel[n][j][1]) * (pixel[m][j][1] - pixel[n][j][1]);
                g = (pixel[m][j][2] - pixel[n][j][2]) * (pixel[m][j][2] - pixel[n][j][2]);
                x = r + b + g;
            }
            int m1 = (j - 1 + cols) % cols;
            int n1 = (j + 1) % cols;
            {
                r = (pixel[i][m1][0] - pixel[i][n1][0]) * (pixel[i][m1][0] - pixel[i][n1][0]);
                b = (pixel[i][m1][1] - pixel[i][n1][1]) * (pixel[i][m1][1] - pixel[i][n1][1]);
                g = (pixel[i][m1][2] - pixel[i][n1][2]) * (pixel[i][m1][2] - pixel[i][n1][2]);
                y = r + b + g;
            }
            energy_cal2[i][j] = sqrt(x + y);
        }
    }

    return energy_cal2;
}
int **cost_cal(int **energy, int rows, int cols)
{
    int **cost_mat = new int *[rows];
    for (int i = 0; i < rows; i++)
    {
        cost_mat[i] = new int[cols];
    }
    // find cost mat
    cost_mat[0] = energy[0];
    for (int i = 1; i < rows; i++)
    {
        cost_mat[i][0] = energy[i][0] + min(cost_mat[i - 1][0], cost_mat[i - 1][1]);
        for (int j = 1; j < cols - 1; j++)
        {
            cost_mat[i][j] = energy[i][j] + min(min(cost_mat[i - 1][j], cost_mat[i - 1][(j + 1) % cols]), cost_mat[i - 1][(j - 1 + cols) % cols]);
        }
        cost_mat[i][cols - 1] = energy[i][cols - 1] + min(cost_mat[i - 1][cols - 1], cost_mat[i - 1][cols - 2]);
    }

    // for(int i=0;i<rows;i++)
    // {
    //     for(int j=0;j<cols;j++)
    //     {
    //         cout << cost_mat[i][j]<< " ";
    //     }
    //     cout <<endl;
    // }
    return cost_mat;
}
int *remove_horizontal(int **cost, int **energy, int rows, int cols)
{
    int mini = INT_MAX;
    int *arr = new int[rows];
    int ind;
    for (int j = 0; j < cols; j++)
    {
        if (mini > cost[rows - 1][j])
        {
            mini = cost[rows - 1][j];
            ind = j;
        }
    }
    arr[rows - 1] = ind;
    // cout << ind << endl;
    int i;
    for (int i = rows - 2; i >= 0; i--)
    {
        if (ind == 0)
        {
            if (cost[i][ind] < cost[i][ind + 1])
            {
                ind = ind;
            }
            else
            {
                ind = ind + 1;
            }
            arr[i] = ind;
        }
        else if (ind == cols - 1)
        {
            if (cost[i][ind] < cost[i][ind - 1])
            {
                ind = ind;
            }
            else
            {
                ind = ind - 1;
            }
            arr[i] = ind;
        }
        else
        {
            int temp = min(cost[i][ind], cost[i][ind - 1]);

            int mini = min(cost[i][ind + 1], temp);

            if (mini == cost[i][ind])
            {
                ind = ind;
            }
            else if (mini == cost[i][ind - 1])
            {
                ind = ind - 1;
            }
            else
            {
                ind = ind + 1;
            }

            arr[i] = ind;
        }
    }

    return arr;
}

int *remove_vertical(int **cost, int **energy, int rows, int cols)
{
    int mini = INT_MAX;
    int *arr = new int[cols];
    int ind;
    for (int j = 0; j < rows; j++)
    {
        if (mini > cost[j][cols - 1])
        {
            mini = cost[j][cols - 1];
            ind = j;
        }
    }
    arr[cols - 1] = ind;
    int i;
    for (int i = cols - 2; i >= 0; i--)
    {
        if (ind == 0)
        {
            if (cost[ind][i] < cost[ind + 1][i])
            {
                ind = ind;
            }
            else
            {
                ind = ind + 1;
            }
            arr[i] = ind;
        }
        else if (ind == rows - 1)
        {
            if (cost[ind][i] < cost[ind - 1][i])
            {
                ind = ind;
            }
            else
            {
                ind = ind - 1;
            }
            arr[i] = ind;
        }
        else
        {
            int temp = min(cost[ind][i], cost[ind - 1][i]);

            int mini = min(cost[ind + 1][i], temp);

            if (mini == cost[ind][i])
            {
                ind = ind;
            }
            else if (mini == cost[ind - 1][i])
            {
                ind = ind - 1;
            }
            else
            {
                ind = ind + 1;
            }

            arr[i] = ind;
        }
    }

    return arr;
}

int main(int argc, char **argv)
{
    string image_path = argv[1];
    Mat img = imread(image_path);
    int new_rows = stoi(argv[2]);
    int new_cols = stoi(argv[3]);

    if (img.empty())
    {
        cout << "Could not read the image: " << image_path << endl;
        return 1;
    }
    int ***pixel1 = new int **[img.rows];
    for (int i = 0; i < img.rows; i++)
    {
        pixel1[i] = new int *[img.cols];
        for (int k = 0; k < img.cols; k++)
        {
            pixel1[i][k] = new int[3];
        }
    }

    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            pixel1[i][j][0] = img.at<Vec3b>(i, j)[0];
            pixel1[i][j][1] = img.at<Vec3b>(i, j)[1];
            pixel1[i][j][2] = img.at<Vec3b>(i, j)[2];
        }
    }

    int cols = img.cols;
    int counter = 0;
    // cout << cols << endl;e

    while (img.cols > new_cols)
    {
        int **energy = energy_cal(pixel1, img.rows, img.cols);
        int **cost = cost_cal(energy, img.rows, img.cols);
        int *arr = remove_horizontal(cost, energy, img.rows, img.cols);
        int ***pixel2 = new int **[img.rows];
        for (int i = 0; i < img.rows; i++)
        {
            pixel2[i] = new int *[img.cols];
            for (int k = 0; k < img.cols; k++)
            {
                pixel2[i][k] = new int[3];
            }
        }

        for (int i = 0; i < img.rows; i++)
        {
            int b = 0;
            for (int j = 0; j < img.cols; j++)
            {
                if (j != arr[i])
                {
                    pixel2[i][b][0] = img.at<Vec3b>(i, j)[0];
                    pixel2[i][b][1] = img.at<Vec3b>(i, j)[1];
                    pixel2[i][b][2] = img.at<Vec3b>(i, j)[2];
                    b++;
                }
            }
        }

        if (arr != nullptr)
            delete[] arr;

        delete[] cost;
        delete[] pixel1;
        delete[] energy;
        pixel1 = pixel2;
        img.cols--;

        Mat img2(img.rows, img.cols, CV_8UC3);
        for (int i = 0; i < img.rows; i++)
        {
            for (int j = 0; j < img.cols; j++)
            {
                img2.at<Vec3b>(i, j)[0] = static_cast<uchar>(pixel1[i][j][0]);
                img2.at<Vec3b>(i, j)[1] = static_cast<uchar>(pixel1[i][j][1]);
                img2.at<Vec3b>(i, j)[2] = static_cast<uchar>(pixel1[i][j][2]);
            }
        }

        // delete[] img;
        img = img2;
    }

    while (img.rows > new_rows)
    {
        int **energy = energy_cal(pixel1, img.rows, img.cols);
        int **cost = cost_cal(energy, img.rows, img.cols);
        int *arr = remove_vertical(cost, energy, img.rows, img.cols);
        int ***pixel2 = new int **[img.rows];
        for (int i = 0; i < img.rows; i++)
        {
            pixel2[i] = new int *[img.cols];
            for (int k = 0; k < img.cols; k++)
            {
                pixel2[i][k] = new int[3];
            }
        }
        for (int j = 0; j < img.cols; j++)
        {
            int b = 0;
            for (int i = 0; i < img.rows; i++)
            {
                if (i != arr[i])
                {
                    pixel2[b][j][0] = img.at<Vec3b>(i, j)[0];
                    pixel2[b][j][1] = img.at<Vec3b>(i, j)[1];
                    pixel2[b][j][2] = img.at<Vec3b>(i, j)[2];
                    b++;
                }
            }
        }

        if (arr != nullptr)
            delete[] arr;

        delete[] cost;
        delete[] pixel1;
        delete[] energy;
        pixel1 = pixel2;
        img.rows--;

        Mat img2(img.rows, img.cols, CV_8UC3);
        for (int i = 0; i < img.rows; i++)
        {
            for (int j = 0; j < img.cols; j++)
            {
                img2.at<Vec3b>(i, j)[0] = static_cast<uchar>(pixel1[i][j][0]);
                img2.at<Vec3b>(i, j)[1] = static_cast<uchar>(pixel1[i][j][1]);
                img2.at<Vec3b>(i, j)[2] = static_cast<uchar>(pixel1[i][j][2]);
            }
        }

        // delete[] img;
        img = img2;
    }

    imwrite("output.jpeg", img);
    return 0;
}