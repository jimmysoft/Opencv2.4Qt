#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "QTimer"
//#include "iostream"

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QTimer *timer;
void MainWindow::on_pushButton_clicked()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(captureVideo()));
    timer->start(10);
}

void MainWindow::on_pushButton_2_clicked()
{
    timer->stop();
}

void MainWindow::captureVideo()
{
     VideoCapture cap(0);
     Mat image;
     cap >> image;

     QImage qimg(image.data, image.cols, image.rows, image.step, QImage::Format_RGB32);
     ui->label->setPixmap(QPixmap::fromImage(qimg));
}


void MainWindow::on_pushButton_3_clicked()
{
    CvCapture *capture = 0;
    IplImage  *frame = 0;
    int       key = 0;

    /* initialize camera */
    capture = cvCaptureFromCAM( 0 );

    /* always check
    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return 1;

    }*/

    /* create a window for the video */

   // cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );

    while( key != 'q' ) {
        /* get a frame */
        frame = cvQueryFrame( capture );

        /* always check */
        if( !frame ) break;

        /* display current frame */
        //cvShowImage( "result", frame );

        Mat imgMat(frame);
        imshow("Mat", imgMat);

        Canny(imgMat, imgMat, 100, 200, 5);
        std::vector<std::vector<cv::Point> > contours;
        findContours(imgMat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        vector<Moments> mu(contours.size());

        for (int i = 0; i < contours.size(); i++)
        {
            mu[i] = moments(contours[i], false);
        }
        //  Get the mass centers:
        vector<Point2f> mc(contours.size());
        for (int i = 0; i < contours.size(); i++)
        {
            mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
        }
        //contours to point iterator
        std::vector<std::vector<cv::Point> >::iterator it = contours.begin();
        int i = 0;
        //change color gray -> CV_8UC3
        Mat drawing = Mat::zeros(imgMat.size(), CV_8UC3);
        //draw line and center
        while (it != contours.end())
        {
            //poly vector
            std::vector<cv::Point> poly;
            //get poly vector from points (poly line length)
            cv::approxPolyDP(*it, poly, 10, true);
            //is it square?
            //if (poly.size() == 4)
            if (poly.size() >= 4 && fabs(contourArea(Mat(poly))) > 1000 && isContourConvex(Mat(poly)))
            //if (poly.size() >= 4)
            {
                //draw lines
                //cv:polylines(result, poly, true, Scalar(255,255,0), 30);
                cv:polylines(drawing, poly, true, Scalar(255, 255, 0), 5);
                cv::rectangle(drawing, poly[0], poly[2], CV_RGB(0, 255, 0), 2);
                //draw centers
                circle(drawing, mc[i], 4, Scalar(100, 200, 0), -1, 8, 0);
            }
            ++it;
            i++;
        }

        imshow("contours", drawing);




        /* exit if user press 'q' */
        key = cvWaitKey( 1 );

    }


    /* free memory */
    cvDestroyWindow( "result" );
    cvReleaseCapture( &capture );
}
