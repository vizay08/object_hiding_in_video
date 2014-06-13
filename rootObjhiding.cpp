//object hiding root project
//this removes the selected object

#include <opencv2\opencv.hpp>
#define WHITE 255
#define BLACK 0

bool isDown = false;
bool started = false;
bool inDebugMode = true;
bool isPainting = false;
bool removeObjectMode = false;
bool srcKeypointsDetected = false;
cv::Mat image;
int px=-1,py=-1;
int minHessian = 500;
cv::Rect objectBoundingRectangle = cv::Rect(0,0,0,0);
int theObject[2] = {0,0};
int framecount= 0;

void help()
{
	cout<<"=============================Object hiding in Video:===========================\n"
		"This program is used to hide the remove the objects in a video.\n There are two modes->\n\tUserMode\n\tProogrammer's Mode\n"
		"Press h for help\n"
		"Press r to remove the object\n"
		"Press d For enabling/disabling edebug mode\n"
		"Press c for painting mode\n"
		"Press s for start the removal part\n"
		"Press p for pausing the application\n"
		"Press i to increase the rate\n"
		"Press o to decrease the frame rate\n"
		"Press t to increase sensitivity value\n"
		"Press y to decrease the Sensitivity Value\n"
		"Press Esc for escape\n"
		"===========================XXXXXXXXXXXXXXXXXXXXXXXXXXXX========================"
		<<endl;
}


cv::Mat removeObjectbyFrameReplacement(cv::Mat currentFrame,cv::Mat frames[],cv::Mat threshold)
{

	cout<<"remove object by frame replacement started"<<endl;
	cv::Mat returnImage;
	cv::Mat framesinFunction[] = {frames[0],frames[1],frames[2],frames[3],frames[4]};

	int x = objectBoundingRectangle.x,y = objectBoundingRectangle.y,width = objectBoundingRectangle.width,height = objectBoundingRectangle.height
		,maxRed,maxGreen,maxBlue;
	int r=-1,g=-1,b=-1;
	int ra[5],ga[5],ba[5];
	///search for previous frames for that position data
	for(int i=x;i<x+width&&i<currentFrame.cols;i++)
	{
		for(int j=y;j<y+height&& j<currentFrame.rows;j++)
		{

			cout<<"Its fine"<<i<<" "<<j<<endl;
			if(threshold.at<uchar>(j,i) == BLACK)
				continue;
			else
			{///check the previous frames whether if any possibility to replace the pixel

					r = -1,g = -1 ,b =-1;
					b = currentFrame.at<cv::Vec3b>(j,i)[0];
					g = currentFrame.at<cv::Vec3b>(j,i)[1];
					r = currentFrame.at<cv::Vec3b>(j,i)[2];

					cout<<"currentframe"<<r<<" "<<g<<" "<<b<<endl;
					for(int fc = 0;fc<5;fc++)
					{
						
						
						ra[fc] = framesinFunction[fc].at<cv::Vec3b>(j,i)[2];
						ga[fc] = framesinFunction[fc].at<cv::Vec3b>(j,i)[1];
						ba[fc] = framesinFunction[fc].at<cv::Vec3b>(j,i)[0];

						cout<<"frames-"<<fc<<"red"<<ra[fc]<<" "<<ga[fc]<<" "<<ba[fc]<<endl;
					}


					int temp;
					//sorting the arrays
					for(int s=0;s<5;s++)
					{
						for(int t=s;t<5;t++)
						{
							if(ra[s]>ra[t])
							{
								temp = ra[s];
								ra[s] = ra[t];
								ra[t] = temp;
							}
						}
					}

					for(int s=0;s<5;s++)
					{
						for(int t=s;t<5;t++)
						{
							if(ga[s]>ga[t])
							{
								temp = ga[s];
								ga[s] = ga[t];
								ga[t] = temp;
							}
						}
					}
						for(int s=0;s<5;s++)
					{
						for(int t=s;t<5;t++)
						{
							if(ba[s]>ba[t])
							{
								temp = ba[s];
								ba[s] = ba[t];
								ba[t] = temp;
							}
						}

					}
						////count the max iterated items
						 int valArray[5]={-1,-1,-1,-1,-1},countArray[5]= {1,1,1,1,1};
						int index = 0;
						int max = 256;
						for(int s=0;s<5;s++)
						{
                      
							
							if(ra[s]==valArray[index])
							{
								countArray[index]++;
							}
							else
							{
								index++;
								valArray[index] = ra[s];
							}
						
						}
						int maxVal;
						max = -1;
						for(int i=1;i<=index+1;i++)
					    {
								if(countArray[i] > max)
								{
									max = countArray[i];
									maxVal = valArray[i];
								}
        //cout<<countArray[i]<<" "<<valArray[i]<<endl;
						 }
						maxRed = maxVal;
			
			

						
						for(int i=0;i<5;i++)
						{
							valArray[i]=-1;
							countArray[i]=1;
						}
						index = 0;
						
						for(int s=0;s<5;s++)
						{
                      
							
							if(ra[s]==valArray[index])
							{
								countArray[index]++;
							}
							else
							{
								index++;
								valArray[index] = ga[s];
							}
						
						}
						
						max = -1;
						for(int i=1;i<=index;i++)
					    {
								if(countArray[i] > max)
								{
									max = countArray[i];
									maxVal = valArray[i];
								}
        //cout<<countArray[i]<<" "<<valArray[i]<<endl;
						 }
						maxGreen = maxVal;


						for(int i=0;i<5;i++)
						{
							valArray[i]=-1;
							countArray[i]=1;
						}
						index = 0;
						
						for(int s=0;s<5;s++)
						{
                      
							
							if(ra[s]==valArray[index])
							{
								countArray[index]++;
							}
							else
							{
								index++;
								valArray[index] = ba[s];
							}
						
						}
						
						max = -1;
						for(int i=1;i<=index;i++)
					    {
								if(countArray[i] > max)
								{
									max = countArray[i];
									maxVal = valArray[i];
								}
        //cout<<countArray[i]<<" "<<valArray[i]<<endl;
						 }
	
						maxBlue = maxVal;

						returnImage.at<cv::Vec3b>(j,i)[0]=maxBlue;
						returnImage.at<cv::Vec3b>(j,i)[1] = maxGreen;
						returnImage.at<cv::Vec3b>(j,i)[2] = maxRed;

			}
		}
	}
			
	
	return returnImage;
}

cv::Mat removeObject(cv::Mat coloredImage,cv::Mat threshold) // colorImage for intensities --- threshold for confirmation - value deletion
{///////////////////////////////////////////////////

	int rows = coloredImage.rows,cols = coloredImage.cols;
	int patchSize = 3,radius = 4;
	int k = (patchSize)/2,minDistance = 800,tempDistance,tempMul;
	int x = objectBoundingRectangle.x,y= objectBoundingRectangle.y,width = objectBoundingRectangle.width,height =  objectBoundingRectangle.height;
	
	cv::Mat returnImage = coloredImage.clone(),imgGs;
	//cv::pyrDown(returnImage,returnImage);
	
	cv::cvtColor(coloredImage,imgGs,CV_RGB2GRAY);

	imgGs &= ~threshold;
	//cv::pyrDown(imgGs,imgGs);
	//cout<<"loop is going to start"<<endl;

	for(int i=x;i<=x+width;i++)//+)
	{
		for(int j=y;j<=y+height;j++)//+)
		{
		//	cout<<"loop is started i,j "<<i<<" "<<j<<endl;
			

			if(threshold.at<uchar>(j,i) == BLACK)
				continue;
			else
			{
				minDistance = 800;
				for(int ix = i-radius;ix < cols - k+radius;ix++)//+)
				{
					for(int jx = j-radius;jx< rows -k+radius ; jx++)//+)
					{

						//cout<<"Problem here"<<endl;
						if(ix<=0 || ix>=cols-k || jx<=0 ||jx >= rows-k)
							continue;

						if(ix==i || jx == j)
							continue;

						tempDistance  = 0 ;
						for(int subX = -k;subX <= k ; subX++)
						{
							for(int subY = -k ; subY <=k ;subY++)
							{
								if(i+subX >=0 && i+subX<cols && j+subY >=0 && j+subY<rows && ix+subX >=0 && ix+subX<cols && jx+subY >=0 && jx+subY<rows)
								{
								//	cout<<"tempX value"<<endl;
								tempMul = imgGs.at<uchar>(i+subX,j+subY) - imgGs.at<uchar>(jx+subX,ix+subY);
								tempMul *= tempMul;
								
								tempDistance += tempMul;
								
								//cout<<tempDistance<<" "<<tempMul<<endl;
								}
								

								
							}
						}
					//	cout<<"tempDistance"<<tempDistance<<endl;
						tempDistance = (int)sqrt((float)tempDistance);
						//cout<<"tempDistance"<<tempDistance<<" minDistance "<<minDistance<<endl;

						if(tempDistance < minDistance)
						{
							//cout<<"replaced"<<endl;
							minDistance = tempDistance;
							for(int subX = -k;subX <= k ; subX++)
							{
								for(int subY = -k ; subY <=k ;subY++)
									{
										
									if(i+subX >=0 && i+subX<cols && j+subY >=0 && j+subY<rows && ix+subX >=0 && ix+subX<cols && jx+subY >=0 && jx+subY<rows)
											{
										returnImage.at<cv::Vec3b>(i+subX,j+subY)[0] = returnImage.at<cv::Vec3b>(jx+subX,ix+subY)[0];
										returnImage.at<cv::Vec3b>(i+subX,j+subY)[1] = returnImage.at<cv::Vec3b>(jx+subX,ix+subY)[1];
										returnImage.at<cv::Vec3b>(i+subX,j+subY)[2] = returnImage.at<cv::Vec3b>(jx+subX,ix+subY)[2];
											}
										}
									}
							
						}
					}
				}
			}
		}
	}

	return returnImage;
}

void searchForMovement(cv::Mat thresholdImage, cv::Mat &cameraFeed){
	//notice how we use the '&' operator for the cameraFeed. This is because we wish
	//to take the values passed into the function and manipulate them, rather than just working with a copy.
	//eg. we draw to the cameraFeed in this function which is then displayed in the main() function.
	bool objectDetected=false;
	cv::Mat temp;
	thresholdImage.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	//findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );// retrieves all contours
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );// retrieves external contours

	//if contours vector is not empty, we have found some objects
	if(contours.size()>0) objectDetected=true;
	else objectDetected = false;

	if(objectDetected){
		//the largest contour is found at the end of the contours vector
		//we will simply assume that the biggest contour is the object we are looking for.
		vector< vector<cv::Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1));
		//make a bounding rectangle around the largest contour then find its centroid
		//this will be the object's final estimated position.
		objectBoundingRectangle = boundingRect(cv::Mat(largestContourVec.at(0)));
		int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2;
		int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2;

		//update the objects positions by changing the 'theObject' array values
		theObject[0] = xpos , theObject[1] = ypos;

		cout<<"object positions -x"<<objectBoundingRectangle.x<<" y-"<<objectBoundingRectangle.y<<"width: "<<objectBoundingRectangle.width<<"height: "<<objectBoundingRectangle.height<<endl;
	}
	//make some temp x and y variables so we dont have to type out so much
	int x = theObject[0];
	int y = theObject[1];
	//draw some crosshairs on the object
	cv::circle(cameraFeed,cv::Point(x,y),20,cv::Scalar(0,255,0),2);
	cv::line(cameraFeed,cv::Point(x,y),cv::Point(x,y-25),cv::Scalar(0,255,0),2);
	cv::line(cameraFeed,cv::Point(x,y),cv::Point(x,y+25),cv::Scalar(0,255,0),2);
	cv::line(cameraFeed,cv::Point(x,y),cv::Point(x-25,y),cv::Scalar(0,255,0),2);
	cv::line(cameraFeed,cv::Point(x,y),cv::Point(x+25,y),cv::Scalar(0,255,0),2);
	cv::rectangle(cameraFeed,objectBoundingRectangle,cv::Scalar(255,0,0),2);

	//cv::putText(cameraFeed,"Tracking object at (" + intToString(x)+","+intToString(y)+")",Point(x,y),1,1,Scalar(255,0,0),2);



}

void retrieveMinMaxPoints(cv::Mat image,cv::Point* minLoc,cv::Point *maxLoc)
{	
	
	int rows = image.rows,cols = image.cols,temp;
	int minX = cols,minY =rows,maxX = 0,maxY  = 0;
	int sensitivityValue = 20;
	for(int x=1;x<cols-1; x++)
	{
		for(int y=1;y<rows-1;y++)
		{
			temp = image.at<uchar>(cv::Point(x,y));
			if(temp != BLACK)
			{//check if 
				if(image.at<uchar>(cv::Point(x-1,y)) == BLACK)
				{
					if(x<minX)
					minX = x;
				}
				if(image.at<uchar>(cv::Point(x,y-1))== BLACK)
				{
					if(y<minY)
					minY = y;
				}
				if(image.at<uchar>(cv::Point(x,y+1))==BLACK)
				{
					if(y>maxY)
					maxY = y;
				}
				if(image.at<uchar>(cv::Point(x+1,y))==BLACK)
				{
					if(x>maxX)
					maxX = x;
				}
			}
		}
	}

	(*minLoc).x = minX;
	(*minLoc).y = minY;
	(*maxLoc).x = maxX;
	(*maxLoc).y = maxY;
}


void mouseCallback(int event,int x ,int y ,int flags,void* params)
{
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:	
		isPainting = true;
		isDown = true;
		break;
	case CV_EVENT_LBUTTONUP:
		isDown = false;
		px =  -1 ,py =-1;
		break;
	case CV_EVENT_MOUSEMOVE:
		if(isDown)
		{
			px  = x;
			py = y;
			if(inDebugMode)
				cout<<"moving x-"<<x<<" y-"<<y<<endl;
		}
		break;
	}
}

int  main()
{
	int count =0; //number of images in the frames array 
	cv::VideoCapture capture;
	cv::Mat frames[5];
	cv::Mat frame1,frame2,frame1gs,frame2gs,frameTs;
	cv::Mat newFrame;
	cv::Mat frameTemp;
	cv::Mat thresholdImage;
	cv::Mat output;
	int sensitivityValue = 20;
	int blur_size = 20;	
	int frameTime = 80;

	char *s = (char*)malloc(sizeof(char)*100);
	help();

	cout<<"enter the video path"<<endl;

	do{//making sure no error in opening the stream
		gets(s);
		capture.open(s);
	}while(!capture.isOpened());


	
		
	capture.read(frame1);
	//frameTemp = frame1.zeros(frame1.rows,frame1.cols,CV_8UC1);
	char choice;
//	cv::namedWindow("test");  // will be  changed
	cout<<"frame size-rows: "<<frame1.rows<<"cols: "<<frame1.cols<<endl;
	while(1)
	{
		capture.read(frame1);  //reading frame1
		if(frame1.empty())
			exit(0);
		cv::cvtColor(frame1,frame1gs,CV_RGB2GRAY); //converting frame1 into grayscale
	
		if(count < 5){
			frames[count++] = frame1;
			
		}
		else
		{
			for(int something = 4;something >0 ;something--)
			{
				frames[something]= frames[something-1];
			}
			frames[0] = frame1;
		}
		capture.read(frame2);
		if(count < 5){
			frames[count++] = frame2;
			
		}
		else
		{
			for(int something = 4;something >0 ;something--)
			{
				frames[something]= frames[something-1];
			}
			frames[0] = frame2;
		}
		newFrame = frame2.clone();
		if(frame2.empty())
			exit(0);
		cv::cvtColor(frame2,frame2gs,CV_RGB2GRAY);  // converting frame2 into grayscale


		cv::absdiff(frame1gs,frame2gs,frameTs);

		cv::threshold(frameTs,thresholdImage,sensitivityValue,255,cv::THRESH_BINARY);

		cv::blur(thresholdImage,thresholdImage,cv::Size(20,20));

		cv::threshold(thresholdImage,thresholdImage,sensitivityValue,255,cv::THRESH_BINARY);


		searchForMovement(thresholdImage,frame2);


		cv::imshow("difference",thresholdImage);	
		if(removeObjectMode){
			
			cv::inpaint(newFrame,thresholdImage,output,1,CV_INPAINT_TELEA);

		//output = removeObject(newFrame,thresholdImage);

			//output = removeObjectbyFrameReplacement(newFrame,frames,thresholdImage);

		if(!output.empty())
		cv::imshow("removed object",output);
		}
		else
		{
			cv::destroyWindow("removed object");
		}
		if(inDebugMode){
		cv::imshow("image tracking",frame2);
		}
		else
		{
			cv::destroyWindow("image tracking");
		}
		if(frame1.empty()||frame2.empty())
			exit(0);

		
		choice = cv::waitKey(frameTime);

		
		switch(choice)
		{
		case 27:
			exit(0);
			break;
		case 'h':
			help();
			break;
		case 'i':
			frameTime -= 5;
			if(frameTime<10)
				frameTime = 10;	
			break;

		case 'o':
			frameTime+=5;
			if(frameTime>100)
				frameTime = 100;
			
			break;


		case 'r':
			removeObjectMode = !removeObjectMode;
			if(removeObjectMode)
			{
				cout<<"remove object mode enabled"<<endl;
			}
			else
			{
				cout<<"remove object mode disabled"<<endl;
			}
			break;
		case 'p':
			cv::waitKey();
			break;


		case 'c':
			isPainting =! isPainting;
			if(isPainting)
				cout<<"painting mode is disabled"<<endl;
			else
				cout<<"painting mode is enabled"<<endl;
			
			break;

		case 't':
			sensitivityValue +=10;
			if(sensitivityValue > 100)
				sensitivityValue = 100;
			break;
		case 'y':
			sensitivityValue -=10;
			if(sensitivityValue < 20)
				sensitivityValue = 20;
			break;
		case 'd':
			if(inDebugMode)
			{
				cout<<"debugging mode disabled"<<endl;
			}
			else
			{
				cout<<"debugging mode enabled"<<endl;
				cout<<"frame rate : "<<abs(1000/frameTime)<<endl;
				
			}
			inDebugMode = !inDebugMode;	   //this will enable/disable debuggingg mode

			break;
		}

		if(isPainting){
		if(px!=-1 &&py!=-1){
		//cv::circle(frame,cv::Point(px,py),20,cv::Scalar(0,255,0),-4);
		//cv::circle(frameTemp,cv::Point(px,py),20,cv::Scalar(255),-4);
		}
		//cv::imshow("video temp",frameTemp);
		}

		//cv::imshow("video",frame);


		framecount++;
	}

	return 0;
}