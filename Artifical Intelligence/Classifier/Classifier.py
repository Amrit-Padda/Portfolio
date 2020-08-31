import csv
import string
from math import log 
import operator
from sklearn.metrics import precision_recall_fscore_support
import matplotlib.pyplot as plt
import numpy as np

#This method will build the model file
def build_model():
    #data = {'word": {'story':2 , 'ask':3}}
    data = {}
    #counts = [story, show, ask, poll]
    counts = [0,0,0,0]
    ctr = 0
    with open('hns_2018_2019.csv', encoding="utf8") as csvfile:
        csv_read = csv.reader(csvfile, delimiter=',')
        for line in csv_read:
            if '2018' in line[5]:
                ctr = ctr + 1
                if line[3] == 'show_hn':
                    counts[1] = counts[1] + 1
                if line[3] == 'story':
                    counts[0] = counts[0] + 1
                if line[3] == 'ask_hn':
                    counts[2] = counts[2] + 1
                if line[3] == 'poll':
                    counts[3] = counts[3] + 1

                for word in line[2].split():
                    word = word.lower()
                    word = word.translate(str.maketrans('', '', string.punctuation))
                    if word in data:
                        if line[3] in data[word]:
                            data[word][line[3]] = data[word][line[3]] + 1
                        else:
                            data[word][line[3]] = 1
                    else:
                        data[word] = {}
                        data[word][line[3]] = 1
        return data, counts
        
#This method will build the model file with stopwords
def build_model_stopwords():
    #data = {'word": {'story':2 , 'ask':3}}
    data = {}
    #counts = [story, show, ask, poll]
    counts = [0,0,0,0]
    ctr = 0
    flag = False
    stopwords = open('stopwords.txt', encoding="utf-8")
    stop_lines = stopwords.readlines()
    with open('hns_2018_2019.csv', encoding="utf8") as csvfile:
        csv_read = csv.reader(csvfile, delimiter=',')
        for line in csv_read:
            if '2018' in line[5]:
                ctr = ctr + 1
                if line[3] == 'show_hn':
                    counts[1] = counts[1] + 1
                if line[3] == 'story':
                    counts[0] = counts[0] + 1
                if line[3] == 'ask_hn':
                    counts[2] = counts[2] + 1
                if line[3] == 'poll':
                    counts[3] = counts[3] + 1

                for word in line[2].split():                   
                    word = word.lower()
                    word = word.translate(str.maketrans('', '', string.punctuation))

                    flag = True
                
                    for line in stop_lines:
                        line = line.strip('\n')
                        line = line.strip()
                        if word in line:
                            flag = False 
                            break
                    
                    if flag:
                        if word in data:
                            if line[3] in data[word]:
                                data[word][line[3]] = data[word][line[3]] + 1
                            else:
                                data[word][line[3]] = 1
                        else:
                            data[word] = {}
                            data[word][line[3]] = 1
                    
        return data, counts

#This method will build the model file with length filtering
def build_model_length():
    #data = {'word": {'story':2 , 'ask':3}}
    data = {}
    #counts = [story, show, ask, poll]
    counts = [0,0,0,0]
    ctr = 0
    flag = False
    with open('hns_2018_2019.csv', encoding="utf8") as csvfile:
        csv_read = csv.reader(csvfile, delimiter=',')
        for line in csv_read:
            if '2018' in line[5]:
                ctr = ctr + 1
                if line[3] == 'show_hn':
                    counts[1] = counts[1] + 1
                if line[3] == 'story':
                    counts[0] = counts[0] + 1
                if line[3] == 'ask_hn':
                    counts[2] = counts[2] + 1
                if line[3] == 'poll':
                    counts[3] = counts[3] + 1

                for word in line[2].split():                   
                    word = word.lower()
                    word = word.translate(str.maketrans('', '', string.punctuation))

                    flag = True
                    if len(word) >= 9 or len(word) <= 2:
                        flag = False 

                    
                    if flag:
                        if word in data:
                            if line[3] in data[word]:
                                data[word][line[3]] = data[word][line[3]] + 1
                            else:
                                data[word][line[3]] = 1
                        else:
                            data[word] = {}
                            data[word][line[3]] = 1
                    
        return data, counts

#This method will build the model file with infrequent word filtering
def build_model_infrequent(freq):
    #data = {'word": {'story':2 , 'ask':3}}
    data = {}
    #counts = [story, show, ask, poll]
    counts = [0,0,0,0]
    to_delete = []
    ctr = 0
    flag = False
    with open('hns_2018_2019.csv', encoding="utf8") as csvfile:
        csv_read = csv.reader(csvfile, delimiter=',')
        for line in csv_read:
            if '2018' in line[5]:
                ctr = ctr + 1
                if line[3] == 'show_hn':
                    counts[1] = counts[1] + 1
                if line[3] == 'story':
                    counts[0] = counts[0] + 1
                if line[3] == 'ask_hn':
                    counts[2] = counts[2] + 1
                if line[3] == 'poll':
                    counts[3] = counts[3] + 1

                for word in line[2].split():                   
                    word = word.lower()
                    word = word.translate(str.maketrans('', '', string.punctuation))
                    
                    
                    if word in data:
                        if line[3] in data[word]:
                            data[word][line[3]] = data[word][line[3]] + 1
                        else:
                            data[word][line[3]] = 1
                        
                        if 'freq' in data[word]:
                            data[word]['freq'] = data[word]['freq'] + 1
                        else:
                            data[word]['freq'] = 1
                    else:
                        data[word] = {}
                        data[word][line[3]] = 1
                        data[word]['freq'] = 1

        for word in data:
            if data[word]['freq'] <= freq:
                to_delete.append(word)

        for rem in to_delete:
            del data[rem]
        return data, counts, len(data)

#This method will build the model file with infrequency filtering
def build_model_infrequent_percent(freq):
    #data = {'word": {'story':2 , 'ask':3}}
    data = {}
    #counts = [story, show, ask, poll]
    counts = [0,0,0,0]
    ctr = 0
    flag = False
    with open('hns_2018_2019.csv', encoding="utf8") as csvfile:
        csv_read = csv.reader(csvfile, delimiter=',')
        for line in csv_read:
            if '2018' in line[5]:
                ctr = ctr + 1
                if line[3] == 'show_hn':
                    counts[1] = counts[1] + 1
                if line[3] == 'story':
                    counts[0] = counts[0] + 1
                if line[3] == 'ask_hn':
                    counts[2] = counts[2] + 1
                if line[3] == 'poll':
                    counts[3] = counts[3] + 1

                for word in line[2].split():                   
                    word = word.lower()
                    word = word.translate(str.maketrans('', '', string.punctuation))
                    
                    
                    if word in data:
                        if line[3] in data[word]:
                            data[word][line[3]] = data[word][line[3]] + 1
                        else:
                            data[word][line[3]] = 1
                        
                        if 'freq' in data[word]:
                            data[word]['freq'] = data[word]['freq'] + 1
                        else:
                            data[word]['freq'] = 1
                    else:
                        data[word] = {}
                        data[word][line[3]] = 1
                        data[word]['freq'] = 1
        
        sorted_keys= sorted(data, key=lambda x: (data[x]['freq']))
        to_remove = int(freq * len(data))
        for x in range(0, to_remove):
            del data[sorted_keys[x]]

        return data, counts, len(data)

#this method will write the model to a txt file
def write_model(data, counts, filename):
    file = open(filename,"w+", encoding="utf-8")
    vocab = open("vocabulary.txt","w+", encoding="utf-8")
    counter = 0 
    size = len(data)
    for word in data:
        freq_story = 0 
        freq_show = 0
        freq_ask = 0
        freq_poll = 0

        if 'show_hn' in data[word]:
            freq_show = data[word]['show_hn']

        if 'story' in data[word]:
            freq_story = data[word]['story']
        
        if 'ask_hn' in data[word]:
            freq_ask = data[word]['ask_hn']

        if 'poll' in data[word]:
            freq_poll = data[word]['poll']

        smoothing = 0.5
        
        prob_story = (freq_story + smoothing) / (counts[0] + (smoothing * size ))
        prob_show = (freq_show + smoothing) / (counts[1]+ (smoothing * size))
        prob_ask = (freq_ask + smoothing) / (counts[2]+ (smoothing * size))
        prob_poll = (freq_poll + smoothing) / (counts[3]+ (smoothing * size))
        
        string = str(counter) + "  " + word + "  " +  str(freq_story) + "  " + str(prob_story) + "  " + str(freq_ask) + "  " + str(prob_ask) + "  " + str(freq_show) + "  " + str(prob_show) + "  " + str(freq_poll) + "  " + str(prob_poll)
        vocab.write(word + "\n")
        file.write(string + "\n")
        counter = counter + 1

#this is a helper for the bayes calculation
def bayes_helper(line, model, pos):
    sum = 0 
    for word in line[2].split():
        word = word.lower()
        word = word.translate(str.maketrans('', '', string.punctuation))        
        for model_line in model:
            model_line = model_line.strip('\n')
            model_line = model_line.split()
            if model_line[1] == word:
                sum = sum + log(10, float(model_line[pos]))
    return sum

#this is a helper for log values
def log_helper(base, val):
    if val <= 0:
        return 0
    else:
        return log(base, val)

#this is where the bayes value is calculated
def naive_bayes(counts, line, model, counter):

    p_story = counts[0] / sum(counts)
    p_show = counts[1] / sum(counts)
    p_ask = counts[2] / sum(counts)
    p_poll = counts[3]/ sum(counts)
    
    bayes_story = log_helper(10, p_story) + bayes_helper(line, model, 3)
    bayes_show = log_helper(10, p_show) + bayes_helper(line, model, 7)
    bayes_ask = log_helper(10, p_ask) + bayes_helper(line, model, 5)
    bayes_poll = log_helper(10, p_poll) + bayes_helper(line, model, 9)

    values = {'story' : bayes_story, 'ask-hn' : bayes_ask, 'show-hn' : bayes_show, 'poll' : bayes_poll}

    
    title_type = min(values.items(), key=operator.itemgetter(1))[0]

    if line[3] == title_type:
        correct = 'Right'
    else:
        correct = 'Wrong'
    
    result = str(counter) + "  " + line[2] + "  " + title_type + "  " + str(bayes_story) + "  " + str(bayes_ask) + "  " + str(bayes_show) + "  " + str(bayes_poll) + "  " + correct
    return result      

#same as above however here we return the values instead of writing to a file
def naive_bayes_return(counts, line, model, counter):

    p_story = counts[0] / sum(counts)
    p_show = counts[1] / sum(counts)
    p_ask = counts[2] / sum(counts)
    p_poll = counts[3]/ sum(counts)
    
    bayes_story = log_helper(10, p_story) + bayes_helper(line, model, 3)
    bayes_show = log_helper(10, p_show) + bayes_helper(line, model, 7)
    bayes_ask = log_helper(10, p_ask) + bayes_helper(line, model, 5)
    bayes_poll = log_helper(10, p_poll) + bayes_helper(line, model, 9)

    values = {'story' : bayes_story, 'ask-hn' : bayes_ask, 'show-hn' : bayes_show, 'poll' : bayes_poll}

    title_type = min(values.items(), key=operator.itemgetter(1))[0]

    return title_type, line[3]
    
#This is the classifier that uses the bayes model above
def ml_classifier(counts, resultname, modelname):
    csvfile = open('hns_2018_2019.csv', encoding="utf8")  
    model = open(modelname, encoding="utf8") 
    result = open(resultname,"w+", encoding="utf-8")
    csv_read = csv.reader(csvfile, delimiter=',')
    counter = 0
            
    for line in csv_read:
        if '2019' in line[5]:
            resultin = naive_bayes(counts, line, model, counter)
            result.write(resultin + "\n")
            counter = counter + 1

    csvfile.close()
    model.close()
    result.close()    

#This is the classifier that uses the bayes model above, we return values here instrad of writing
def ml_classifier_return(counts, modelname):
    pred = []
    true = []
    csvfile = open('hns_2018_2019.csv', encoding="utf8")  
    model = open(modelname, encoding="utf8") 
    csv_read = csv.reader(csvfile, delimiter=',')
    counter = 0
            
    for line in csv_read:
        if '2019' in line[5]:
            bayes_pred, bayes_true = naive_bayes_return(counts, line, model, counter)
            pred.append(bayes_pred)
            true.append(bayes_true)
            counter = counter + 1

    csvfile.close()
    model.close()                                   
    return pred, true
    
#----------------------- Task 1 and 2 ------------------------          

model_data, model_counts = build_model()
write_model(model_data, model_counts, 'model-2018.txt')
ml_classifier(model_counts, 'baseline-result.txt', 'model-2018.txt' )

#------------------- ---- Experiment 1 -----------------------     

model_data, model_counts = build_model_stopwords()
write_model(model_data, model_counts, 'stopword-model.txt')
ml_classifier(model_counts, 'stopword-result.txt', 'stopword-model.txt' )

#------------------------ Experiment 2 -----------------------     

model_data, model_counts = build_model_length()
write_model(model_data, model_counts, 'wordlength-model.txt')
ml_classifier(model_counts, 'wordlength-result.txt', 'wordlength-model.txt' )

#-------------------Experiment 3 Frequency--------------------    

percents = [1,5,10,15,20]
freq_np = np.array([[0.0,0.0,0.0,0.0,0.0],
                    [0.0,0.0,0.0,0.0,0.0],
                    [0.0,0.0,0.0,0.0,0.0],
                    [0.0,0.0,0.0,0.0,0.0]])
pos = 0
for percent in percents:
    model_data, model_counts, vocab_size = build_model_infrequent(percent)
    write_model(model_data, model_counts, 'infrequent-model.txt')
    pred, true = ml_classifier_return(model_counts, 'infrequent-model.txt' )
    performance = precision_recall_fscore_support(true, pred, average='macro')
    freq_np[0][pos] =  vocab_size
    freq_np[1][pos] = performance[0]
    freq_np[2][pos] = performance[1]
    freq_np[3][pos] = performance[2]
    pos = pos + 1

#------------------ Experiment 3 Percentage -------------------  

percents = [0.01, 0.05, 0.1, 0.15, 0.2]
freq_pct_np = np.array([[0.0,0.0,0.0,0.0,0.0],
                    [0.0,0.0,0.0,0.0,0.0],
                    [0.0,0.0,0.0,0.0,0.0],
                    [0.0,0.0,0.0,0.0,0.0]])
pos = 0
for percent in percents:
    model_data, model_counts, vocab_size = build_model_infrequent_percent(percent)
    write_model(model_data, model_counts, 'infrequent-percent-model.txt')
    pred, true = ml_classifier_return(model_counts, 'infrequent-percent-model.txt' )
    performance_freq = precision_recall_fscore_support(true, pred, average='macro')
    freq_pct_np[0][pos] =  vocab_size
    freq_pct_np[1][pos] = performance_freq[0]
    freq_pct_np[2][pos] = performance_freq[1]
    freq_pct_np[3][pos] = performance_freq[2]
    pos = pos + 1


plt.subplot(1, 2, 1)
plt.plot(freq_np[0], freq_np[1], '-o')
plt.plot(freq_np[0], freq_np[2], '-o')
plt.plot(freq_np[0], freq_np[3], '-o')
plt.legend(['Precision', 'Recall', 'F-score'], loc='upper left')
plt.title('Frequency based word removal')
plt.xlabel('Words removed')

plt.subplot(1, 2, 2)
plt.plot(freq_pct_np[0], freq_pct_np[1], '-o')
plt.plot(freq_pct_np[0], freq_pct_np[2], '-o')
plt.plot(freq_pct_np[0], freq_pct_np[3], '-o')
plt.legend(['Precision', 'Recall', 'F-score'], loc='upper left')
plt.title('Percentage based word removal')
plt.xlabel('Words removed')
plt.show()