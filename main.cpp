#include"NineSquarePuzzle.h"

int main()
{
	string FilePaths = "D:\NineSquareMat";
	vector<Mat> imgs;
	ReadImg(FilePaths, imgs);
	ReSize(imgs, 600, 600);

	size_t num;
	cout << "***************************************" << endl;
	cout << "ÇëÑ¡ÔñÍ¼Æ¬ÐòºÅ:"; cin >> num;
	cout << "***************************************" << endl;
	assert(num <= imgs.size());
    
	ImgPuzzles::SplitMats(imgs[num - 1]);
	ImgPuzzles::CreatePuzzlesMat();
	waitKey(0);
}