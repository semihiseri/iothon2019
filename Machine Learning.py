import pandas as pd
import  math
# numpy will let us use arrays as python doesnt have arrays
import numpy as np
# preprocessing is done to bring the values in the feature vector between (-1,1) else the entire matrix will be influenced by the higher values
from sklearn import preprocessing
#cross validation is used to split and test the training and testing sets, also help seperate the data
# train_test_split is used to create the split between the training and testing set
#from sklearn.model_selection import cross_validate
from sklearn.model_selection import train_test_split
#support vector machine can be used to do regression as well, we will also look how easy it is to change the algorithms
from sklearn import svm
# importing linear regression 
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures
import datetime # importing date and time
import matplotlib.pyplot as plt #importing graphs
from matplotlib import style

style.use('ggplot') # specific graph plot style to be used

#pickling saves time, serilization of any python object.
#In machine learning case its not practical to train data every time when you want to predict it might take lot of time
#pickling saves the data into the current working directory and we can open it whenever it's required, in addition the model could be retrained once in a month or so
import  pickle
from sklearn.gaussian_process import GaussianProcessRegressor
from sklearn.gaussian_process.kernels \
import RBF, WhiteKernel, RationalQuadratic, ExpSineSquared


df = pd.read_csv("/dbfs/mnt/log-files/FI_Power 2016to.csv")
df=df.tail(5000)

forecast_col = 'PC'
#replace cells which has value missing. In machine learning we cannot work with imporoper matrices
#df.fillna(-99999,inplace=True)

print(df.head(10))

#defining how many predictions are to obtained in the output. In this case 1 %
forecast_out = int(math.ceil(0.01*len(df)))
print(forecast_out)

# we create a label,which is the Adj. Close which is shifted 1% of total length which is aroundn 30 days as total data set 
df['label']=df[forecast_col].shift(-forecast_out)

# feature vector x is labelled as everything other than 'label' column. df.drop  returns a new data frame without 'label'
x = np.array(df.drop(['Date','label'],1))
print(x)

# features are generally represented by x and labels are represented by y
# we use preprocessing to bring the values in the feature vector inside (-1,1) before feeding it into the classifier, this is ginna add to the time
x = preprocessing.scale(x)

# The way I have understood is that those rows until those in forecast_out to be removed
x = x[:-forecast_out]
# x lately would store the data which is to be removed as per the 10 percent
x_lately = x[-forecast_out:]

# since we have shifted the rows of Adj. Close by 30 to get label, we will not have the 30 rows labels we are gonna drop that
df.dropna(inplace=True)
y = np.array(df['label'])

# training set size definition
# we are defining training set and testing set randomly and the split has been decided as 20 % of the entire test case
x_train, x_test, y_train, y_test = train_test_split(x,y, test_size=0.1)




#clf = GaussianProcessRegressor()
#clf.fit(x_train, y_train)



#we use the classsifier here, we could switch over the classifiers here. The Linear regression and SVM have been switched in this case
#clf = LinearRegression(n_jobs=-1) #n_jobs will decide how many jobs needs to be run parallely, to make the training significantly faster. (-1) means it takes as many as supported by the system. we could also put it a number n_jobs =10 
#clf = svm.SVR(kernel='sigmoid') #by default scm have a liner regression, we can change to poly
#ml system training here
#clf.fit(x_train,y_train)

#with open("/dbfs/mnt/log-files/prediction.pickle",'wb') as f:
#    pickle.dump(clf,f)


#NOTE: THE ONE WITH 3 HASH SYMBOLS IS THE TRAINING CODE WHICH IS TO BE RUN ONLY DURING RETRAINING
#This is the code line where the data is been stored after training, so that the system doesnt have to retrain every time we need to predict(We will comment out the previous lines in this cell block after running it for training. pickle file will be saved in the current working directory)    
#So when we rent out the cloud we just need to train the system as fast as possible and take the pickle file. Because training is the place we need GPU
pickle_in = open("/dbfs/mnt/log-files/prediction.pickle",'rb')
clf = pickle.load(pickle_in )

    

#ml system testing here
accuracy = clf.score(x_test,y_test)


print(accuracy)


# now we are trying to predict/ forecast. A single value or array could be passed.
forecast_set = clf.predict(x_lately)

# we are printing the forecast valuefor the next 35days
print(forecast_set, accuracy, forecast_out)


%fs ls /mnt/log-files
