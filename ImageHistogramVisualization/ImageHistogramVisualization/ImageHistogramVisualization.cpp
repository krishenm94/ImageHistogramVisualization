#include <opencv2/opencv.hpp>

#include <iostream>

using namespace std;
using namespace cv;

class histEq16
{
public:
	string src_path, dst_path;

	int cvtImg16()
	{

		// Read image
		Mat img_src = imread(src_path, IMREAD_ANYDEPTH);
		Mat img_dst = img_src.clone();

		//cout << img_src.depth() << " " << img_src.channels() ;

		if (!img_src.data)

		{
			cout << "\nimg_src data in cvtimg16 is empty";
			system("pause");
			return -1;
		}

		if (img_src.depth() != 2)
		{
			cout << "\nimg_src data is not 16 bit";
			system("pause");
			return -1;
		}

		if (img_src.channels() != 1)
		{
			cout << "\nimg_src data is not grayscale, converting...";
			// Convert to grayscale
			cvtColor(img_src, img_src, COLOR_BGR2GRAY);
			return -1;
		}

		// Calculate histogram of source image pixel intensities

		Mat hist;
		int channels = 0;
		int nimgs = 1;
		int dims = 1;
		int bins = 500;

		float range[] = { 0, 65536 };
		const float* ranges = { range };

		calcHist(&img_src, nimgs, &channels, Mat(),
			hist, dims, &bins, &ranges, true, false);


		// Calculating cumulative histogram

		Mat cumhist = hist.clone();

		for (int b = 1; b < bins; b++)
		{
			cumhist.at<float>(b) = hist.at<float>(b) + cumhist.at<float>(b - 1);
		}

		// Cumulative histogram visualization

		//visualiseHist(cumhist, bins, "cumhist");

		// Apply Histogram Equalization
		int k = 0;

		for (int y = 0; y < img_src.rows; y++)
		{
			for (int x = 0; x < img_src.cols; x++)
			{

				k = cvRound(img_src.at<ushort>(y, x) / (65535 / bins));
				//cout << img_src.at<ushort>(y, x) << " -> " << k << " (" << x << "," << y << ")\n";
				img_dst.at<ushort>(y, x) = (ushort)(cvRound(65535 * (cumhist.at<float>(k) / img_src.cols / img_src.rows)));
				//cout << cumhist.at<float>(k) << " " << img_dst.at<ushort>(y, x) << "\n";
			}
		}


		// Write Image to 
		imwrite(dst_path, img_dst);

		cout << "\nHistogram equalization completed";

		return 0;

	}


};

class testImg16
{
public:

	string src_path, dst_path;

	int visualiseHist(Mat hist, int bins, string winName = "hist")
	{
		int const hist_height = 600;
		Mat3b hist_image = Mat3b::zeros(hist_height, bins);

		double max_val = 0;
		minMaxLoc(hist, 0, &max_val);

		// visualize each bin
		for (int b = 0; b < bins; b++)
		{
			float const binVal = hist.at<float>(b);
			int   const height = cvRound(binVal * hist_height / (max_val));
			line
			(hist_image
				, Point(b, hist_height - height), Point(b, hist_height)
				, Scalar::all(bins)
			);
		}

		imshow(winName, hist_image);

		return 0;
	}

	int showImgs()
	{
		// Read images
		Mat img_src = imread(src_path, IMREAD_ANYDEPTH);

		if (!img_src.data)
		{
			cout << "\nimg_src data in showImgs is empty";
			system("pause");
			return -1;
		}

		Mat img_dst = imread(dst_path, IMREAD_ANYDEPTH);
		img_dst = imread("hist16.tif", IMREAD_ANYDEPTH);

		if (!img_dst.data)
		{
			cout << "\nimg_dst data in showImgs is empty";
			system("pause");
			return -1;
		}

		// Display results
		namedWindow("Source Image", WINDOW_NORMAL);
		namedWindow("Equalized Image", WINDOW_NORMAL);

		resizeWindow("Source Image", 600, 600);
		resizeWindow("Equalized Image", 600, 600);

		imshow("Source Image", img_src);
		imshow("Equalized Image", img_dst);

		Mat hist;
		int channels = 0;
		int nimgs = 1;
		int dims = 1;
		int bins = 500;

		float range[] = { 0, 65536 };
		const float* ranges = { range };

		calcHist(&img_src, nimgs, &channels, Mat(),
			hist, dims, &bins, &ranges, true, false);

		visualiseHist(hist, bins);

		Mat histEq;

		calcHist(&img_dst, nimgs, &channels, Mat(),
			histEq, dims, &bins, &ranges, true, false);

		visualiseHist(histEq, bins, "histEq");

		waitKey(0);

		return 0;

	}

};

int main()
{

	cout << "\nPlease enter an image filename to perform hist. eq. on ";
	cout << "\nPress ENTER to use default image: ";
	string src;
	getline(cin, src);

	histEq16 histEq;
	testImg16 testImg;

	if (src.empty()) {
		src = "16bit_grayscale.tif";
	}

	string dst = "hist16.tif";

	histEq.src_path = src;
	histEq.dst_path = dst;

	histEq.cvtImg16();

	testImg.src_path = src;
	histEq.dst_path = dst;

	testImg.showImgs();

	return 0;
}