#!/usr/bin/env python          
      
import time
import serial
import datetime
import httplib,urllib,urllib2
import json
import MySQLdb
import RPi.GPIO as GPIO, time

SORE, MALAM, PAGI, SIANG = 1, 2, 3, 4

global time_now
global date
global times

GPIO.setmode(GPIO.BCM)
headers = {"Content-type": "application/x-www-form-urlencoded","Accept": "text/plain"}
conn = httplib.HTTPConnection("api.thingspeak.com:80")
                
ser = serial.Serial(
              
               port='/dev/ttyUSB0',
               baudrate = 9600,
               #parity=serial.PARITY_NONE,
               #stopbits=serial.STOPBITS_ONE,
               #bytesize=serial.EIGHTBITS,
               #rtscts=True,
               timeout=1
           )
counter=0
ID, I, V, C, Stat, So, StatR, IDSalve = "","","","","","","",""     
urltweet="https://api.thingspeak.com/apps/thingtweet/1/statuses/update"

sore_time = datetime.time(17,45)
malam_time = datetime.time(18,0)
pagi_time = datetime.time(5,35)
siang_time = datetime.time(6,5)
time_to_check = datetime.datetime.now().time()
current_time = datetime.datetime(1,1,1,16,0)
perbaikan = 0
OnCommand = 0
TotalSlave = 1
marker = 0

header =  "|Time"+"\t\t|"+"|ID" +"\t|"+ "Current (A)"+"\t|" + "Voltage (V)" +"\t|" + "Power (Watt)"+"\t|" + "Capacity (%)"+"\t|" +"Status"+"\t\t|" + "Source"+"\t\t|" + "PIR"+"\t\t|" + "Condition|"

def check_time(time_to_check, sore_time, malam_time, pagi_time, siang_time):
    if time_to_check >= sore_time and time_to_check < malam_time:
        return SORE
    elif time_to_check < datetime.time(23,59):
        if time_to_check < pagi_time or time_to_check >= malam_time:
            return MALAM
        elif time_to_check < sore_time and time_to_check >siang_time:
            return SIANG
        elif time_to_check >= pagi_time and time_to_check < siang_time:
            return PAGI


def InputDataPHP(ID,time_now):
    db = MySQLdb.connect(host="192.168.0.100",
                     user="root",
                     passwd="",
                     db="pju_dte")
    cur = db.cursor()

    #perintah sql untuk update data

    
    try :      
        cur.execute ("""INSERT INTO slave_1 (ID, Sumber_daya, Status_nyala, Status_rusak, Date, Time, PIR, Daya, Tegangan, Arus, Baterai)
                        VALUES ( %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)""",(ID, So, Stat, StatR, date, times, PIR, P, V, I, C)) 
        db.commit()
    
    except :  
        db.rollback()
    db.close()
    
def RCtime (PiPin):
  measurement = 0
  # Discharge capacitor
  GPIO.setup(PiPin, GPIO.OUT)
  GPIO.output(PiPin, GPIO.LOW)
  time.sleep(0.1)

  GPIO.setup(PiPin, GPIO.IN)
  # Count loops until voltage across
  # capacitor reads high on GPIO
  while (GPIO.input(PiPin) == GPIO.LOW):
    measurement += 1

  return measurement
    
def writeToFile(stateComp):
    if ID == "1":
        fo = open('slave1.txt','a')
        fo.write(stateComp)
        fo.write('\n')

    elif ID == "2":
        fo = open('slave2.txt','a')
        fo.write(stateComp)
        fo.write('\n')

def read():
    
    global perbaikan
    global ID
    global I
    global V
    global C
    global Stat
    global So
    global PIR
    global StatR
    global P
    global time_now
    global date
    global times
    time_now = datetime.datetime.now()
    date = time_now.date()
    times = time_now.time() 
    x = ser.readline()
    print "x:", x
    Paket = x.split(" ", 11)
    print Paket
    ID = Paket[0]
    I = Paket[2]
    V = Paket[3]
    C = Paket[4]
    Stat = Paket[5]
    So = Paket[6]
    PIR = Paket[7]
    StatR = Paket[8]
    P = float(I)*float(V)
 
    #params = urllib.urlencode({'field1': I,'field2': V, 'field3': P, 'field4': C, 'field5': Stat,'field6': So, 'field7': PIR, 'field8': StatR,'key':'F5NI8CVE4M9UCDMG'})
    #conn.request("POST", "/update", params, headers)
    #response =conn.getresponse()
    #print response.status, response.reason
    #data = response.read()
    #conn.close()

      
    #global state
    if StatR == "3":
        StatR = "Baik"
        perbaikan = 1;
    elif StatR == "0":
        StatR = "Rusak Baterai & Lampu"
        perbaikan = 0
    elif StatR == "1":
        StatR = "Rusak Baterai"
        perbaikan = 0
    elif StatR == "2":
        StatR = "Rusak Lampu"
        perbaikan = 0;

    if PIR == "1":
        PIR = "Aktif"
    else:
        PIR = "Non-Aktif"

    if Stat == "1":
        Stat = "On"
    else:
        Stat = "Off"
    
    if So == "1":
        So = "PLN"
    else:
        So = "Baterai"
    

    #state = "Hari ini: " + str(time_now.hour)+":"+str(time_now.minute)+" " + str(time_now.day) + "/" + str(time_now.month) +"/" + str(time_now.year)+"/" + str(StatR)
    state = "Hari ini: " + str(time_now) + str(StatR)
    if StatR != "3" and perbaikan!=1:
        print "kirim pesan ke Teknisi"
        #urlsms  = "http://freesms4us.com/kirimsms.php?"
        #smsattr = [('user','labtelkom'),('pass','ubayemenz'),('no','087877300496'),('isi',state)]
        #data = urllib.urlencode(smsattr)
        #reqsms = urlsms + data
        #print reqsms
        #reply = urllib2.urlopen(reqsms)
        #print reply.read()

        print "tweet terkirim"
        #tweetparams = {'api_key':'VANS492LMH544F2R','status':state}
        #req = urllib2.Request(urltweet)
        #req.add_header('Content-Type','application/json')
        #data = json.dumps(tweetparams)
        #response = urllib2.urlopen(req,data)
    elif StatR == "3" and perbaikan == 1:
        print "kirim pesan ke Teknisi"
        state = "Hari ini: " + str(time_now)+"/" + "Sudah diperbaiki"
        #urlsms  = "http://freesms4us.com/kirimsms.php?"
        #smsattr = [('user','labtelkom'),('pass','ubayemenz'),('no','087877300496'),('isi',state)]
        #data = urllib.urlencode(smsattr)
        #reqsms = urlsms + data
        #print reqsms
        #reply = urllib2.urlopen(reqsms)
        #print reply.read()

        print "tweet terkirim"
        #tweetparams = {'api_key':'VANS492LMH544F2R','status':state}
        #req = urllib2.Request(urltweet)
        #req.add_header('Content-Type','application/json')
        #data = json.dumps(tweetparams)
        #response = urllib2.urlopen(req,data)
          
      
    stateComp =  "|"+str(time_now)+"\t|"+str(ID) +"\t|"+ str(I)+"\t\t|" + str(V)+"\t\t|" + str(P)+"\t\t|" +str(C)+"\t\t|"+str(Stat)+"\t\t|"+ str(So)+"\t|"+ str(PIR)+"\t|"+str(StatR)+"|"
    print stateComp
    writeToFile(stateComp)
    InputDataPHP(ID,time_now)
    
               
if __name__ == "__main__":
    fo = open('slave1.txt','a')
    fo.write(header)
    fo.write('\n')
    fo = open('slave2','a')
    fo.write(header)
    fo.write('\n')
    while True:
        LDR = RCtime(4)
        print LDR
        #if check_time(time_to_check, sore_time, malam_time, pagi_time, siang_time)== 2 and marker==0:
        if LDR >12 or check_time(time_to_check, sore_time, malam_time, pagi_time, siang_time)== 2 and marker==0:
            OnCommand = 0
            ser.write("?1\n")
            time.sleep(1)
            marker = 1
        else :
            OnCommand = 1
            ser.write("?0\n")
            time.sleep(1)
            marker = 0
        if OnCommand < 2:
            for IDSlave in range (1,TotalSlave+1):
                command = str(IDSlave)+"@\n"
                ser.write(command)
                print command
                read()
        time.sleep(1)
        #ser.close    
