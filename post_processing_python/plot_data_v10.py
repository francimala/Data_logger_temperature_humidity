'''
hf0: diagramma a barre temperatura
hf1: diagramma a barre umidità
hf: andamento temperatura
hf2: andamento umidità
hf3: temperatura data selezionata
hf4: umidità data selezionata
'''

from datetime import datetime, timedelta
import matplotlib.pyplot as plt
import numpy as np
plt.ion() #turn interactive mode on.

fid = open('LOG1.TXT')

#Most of variables are lists
date = []
time = []
T1 = []
T2 = []
H3 = []
N = []
max11 = []
max22 = []
max33 = []
min11 = []
min22 = []
min33 = []
T11 = []
T22 = []
H33 = []
avg11 = []
avg22 = []
avg33 = []
day = []

TS1 = []
TS2 = []
HS3 = []
dateS = '25/5/2018 '
exitS = 0


#With ".append" we add values at the end of the list.
for line in fid: #line è una stringa?
    splitted = line.split(';');
    '''
    Se ho capito bene lui ha salvato all'interno della stringa "line" tutta la riga del file. Ora andiamo a dividere questa stringa in tante
    componenti separate da ';'.
    Domanda: SPLITTED è UNA LISTA?
    '''
    if len(splitted) > 1:
        date.append(splitted[0])
        time.append(splitted[1])
        T1.append(float(splitted[2]))
        T2.append(float(splitted[3]))
        H3.append(float(splitted[4]))
        N.append(int(splitted[5]))

fid.close()

i = 1
count_day = 1

for d1, theta1, theta2, phi3 in zip(date, T1, T2, H3):
    if d1 != date[i] and i < (len(date)-1):
        T11.append(theta1)
        T22.append(theta2)
        H33.append(phi3)
        day.append(d1)
        avg11.append(np.mean(T11))
        avg22.append(np.mean(T22))
        avg33.append(np.mean(H33))
        max11.append(np.amax(T11))
        max22.append(np.amax(T22))
        max33.append(np.amax(H33))
        min11.append(np.amin(T11))
        min22.append(np.amin(T22))
        min33.append(np.amin(H33))

        #We need to clear all previous lists.
        T11 = []
        T22 = []
        H33 = []

        count_day += 1
        i += 1
    elif d1 == date[i]:
        T11.append(theta1)
        T22.append(theta2)
        H33.append(phi3)
        if i < (len(date)-1):
            i += 1

day.append(d1)
avg11.append(np.mean(T11))
avg22.append(np.mean(T22))
avg33.append(np.mean(H33))
max11.append(np.amax(T11))
max22.append(np.amax(T22))
max33.append(np.amax(H33))
min11.append(np.amin(T11))
min22.append(np.amin(T22))
min33.append(np.amin(H33))

print('Average lists:')
print(avg11)
print(avg22)
print(avg33)
print(i)
print(len(date))
print(count_day)
print(day)
print(len(day))

axis_s0 = []
axis_s1 = []

for x0 in range(len(day)):
    axis_s0.append(2*x0)
for x1 in range(len(day)):
    axis_s1.append(2*x1+1)

print(axis_s0)
print(axis_s1)

#Temperature bar
hf0 = plt.figure(facecolor='w', figsize=(12,8))
plt.bar(axis_s0, avg11, color='r', alpha=0.5, label='Average sensor0')
plt.bar(axis_s1, avg22, color='b', alpha=0.5, label='Average sensor1')
plt.bar(axis_s0, max11, color='r', alpha=0.2, label='Max sensor0')
plt.bar(axis_s0, min11, color='r', alpha=1, label='Min sensor0')
plt.bar(axis_s1, max22, color='b', alpha=0.2, label='Max sensor1')
plt.bar(axis_s1, min22, color='b', alpha=1, label='Min sensor1')
plt.title("From " + date[0] + " to " + date[-1], fontsize=14)
plt.legend(loc='best')
plt.xticks(fontsize=14)
plt.yticks(fontsize=18)
plt.xlabel('Time', fontsize=18)
plt.ylabel('Temperature ($^o$C)', fontsize=18)
plt.grid(alpha=0.4)
plt.tight_layout()

axes = plt.gca()
axes.set_ylim([np.amin(T1+T2)-2,np.amax(T1+T2)+2])

plt.xticks(axis_s0, day)
hf0.savefig('theta_bar.png')

#Humidity bar
hf1 = plt.figure(facecolor='w', figsize=(12,8))
plt.bar(axis_s0, min33, color='g', alpha=1, label='Min DHT11')
plt.bar(axis_s0, avg33, color='g', alpha=0.5, label='Average DHT11')
plt.bar(axis_s0, max33, color='g', alpha=0.2, label='Max SHT11')
plt.title("From " + date[0] + " to " + date[-1], fontsize=14)
plt.legend(loc='best')
plt.xticks(fontsize=14)
plt.yticks(fontsize=18)
plt.xlabel('Time', fontsize=18)
plt.ylabel('Humidity (%)', fontsize=18)
plt.grid(alpha=0.5)
plt.tight_layout()
hf1.savefig('phi_bar.png')

DT = [datetime.strptime(d + t, "%d/%m/%Y  %H:%M:%S ") for d,t in zip(date,time)]

#Temperature global
hf = plt.figure(facecolor='w', figsize=(12,6))
plt.plot(DT,T1,'r',linewidth=2,label='sensor 0')
plt.plot(DT,T2,'b',linewidth=2,label='sensor 1')
plt.title("From " + date[0] + " to " + date[-1],fontsize=14) 
plt.xticks(rotation=45)
plt.legend(loc='best')
plt.xticks(fontsize=14)
plt.yticks(fontsize=18)
plt.xlabel('Time',fontsize=18)
plt.ylabel('Temperature ($^o$C)',fontsize=18)
plt.grid()
plt.tight_layout()

hf.savefig('log_theta.png')

#Humidity global
hf2 = plt.figure(facecolor='w', figsize=(12,6))
plt.plot(DT,H3, 'g', linewidth=2, label='sensor DHT11')
plt.title("From " + date[0] + " to " + date[-1], fontsize=14)
plt.xticks(rotation=45)
plt.legend(loc='best')
plt.xticks(fontsize=14)
plt.yticks(fontsize=18)
plt.xlabel('Time', fontsize=18)
plt.ylabel('Humidity (%)', fontsize=18)
plt.grid()
plt.tight_layout()

hf2.savefig('log_phi.png')

for d, theta1, theta2, phi3 in zip (date, T1, T2, H3):
    if d == dateS:
        TS1.append(theta1)
        TS2.append(theta2)
        HS3.append(phi3)
        exitS = 1

if exitS == 0:
    print("La data inserita non è presente nel file delle misurazioni.\n")

if exitS == 1:
    print(TS1)
    print(TS2)
    print(HS3)
    print(np.mean(TS1))
    print(np.mean(TS2))
    print(np.mean(HS3))
    #Temperature
    hf3 = plt.figure(facecolor='w', figsize=(6,7))
    plt.bar(1, np.mean(TS1), color='r', alpha=0.5, label='Average sensor0')
    plt.bar(2, np.mean(TS2), color='b', alpha=0.5, label='Average sensor1')
    plt.bar(1, np.amax(TS1), color='r', alpha=0.2, label='Max sensor0')
    plt.bar(1, np.amin(TS1), color='r', alpha=1, label='Min sensor0')
    plt.bar(2, np.amax(TS2), color='b', alpha=0.2, label='Max sensor1')
    plt.bar(2, np.amin(TS2), color='b', alpha=1, label='Min sensor1')
    plt.title(dateS, fontsize=14)
    plt.legend(loc='best')
    plt.xticks(fontsize=0)
    plt.yticks(fontsize=18)
    plt.xlabel('Date', fontsize=18)
    plt.ylabel('Temperature ($^o$C)', fontsize=18)
    plt.grid(alpha=0.5)
    plt.tight_layout()

    plt.tick_params(
        axis='x',
        which='both',
        bottom='off',
        top='off')
    
    axes = plt.gca()
    axes.set_ylim([np.amin(TS1+TS2)-2,np.amax(TS1+TS2)+2])
    #plt.xticks(1, dateS)

    hf3.savefig('theta_single_day.png')

    #Humidity
    hf4 = plt.figure(facecolor='w', figsize=(5,7))
    plt.bar(1, np.mean(HS3), color='g', alpha=0.5, label='Average DHT11')
    plt.bar(1, np.amax(HS3), color='g', alpha=0.2, label='Max DHT11')
    plt.bar(1, np.amin(HS3), color='g', alpha=1, label='Min DHT11')
    plt.title(dateS, fontsize=14)
    plt.legend(loc='best')
    plt.xticks(fontsize=0)
    plt.yticks(fontsize=18)
    plt.xlabel('Date', fontsize=18)
    plt.ylabel('Humidity (%)', fontsize=18)
    plt.grid(alpha=0.5)
    plt.tight_layout()

    hf4.savefig('phi_single_day.png')
