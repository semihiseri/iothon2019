
# coding: utf-8

# In[6]:


import pandas as pd
import matplotlib.pyplot as plt


# In[7]:


df = pd.read_csv("Prediction.csv")


# In[8]:


print(df.head(10))


# In[9]:


x=df['Time']


# In[10]:


y=df['Energy']


# In[11]:


plt.plot(x, y)
plt.show()


# In[36]:


least=y.min()
print(least)

inx = y.idxmin(axis=0, skipna=True)
print("The Boiler will start working at ", inx ,"th hour slot")

