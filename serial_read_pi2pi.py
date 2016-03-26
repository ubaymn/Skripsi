 #!/usr/bin/env python
          
      
import time
import serial
import time,datetime
import httplib,urllib,urllib2
import json
import RPi.GPIO as GPIO, time

SORE, MALAM, PAGI, SIANG = 1, 2, 3, 4

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
ID, I, V, C, Stat, So, StatR = "","","","","","","",     
urltweet="https://api.thingspeak.com/apps/thingtweet/1/statuses/update"

sore_time = datetime.time(17,45)
malam_time = datetime.time(18,0)
pagi_time = datetime.time(5,35)
siang_time = datetime.time(6,5)
time_to_check = datetime.datetime.now().time()
current_time = datetime.datetime(1,1,1,16,0)
perbaikan = 0
j=0

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

def doit():
    #print "Counter: ",counter
    #counter += 1

    x = ser.readline()
    print "x:", x
    
    i = datetime.datetime.now()
    Paket = x.split(" ", 9)
    ID = Paket[0]
    
    if ID == "1":

      I = Paket[1]
      V = Paket[3]
      C = Paket[4]
      Stat = Paket[5]
      So = Paket[6]
      PIR = Paket[7]
      StatR = Paket[8]
      
      #params = urllib.urlencode({'field1': ID, 'field2': I,'field3': V, 'field4': C, 'field5': Stat,'field6': So, 'field7': PIR, 'key':'F5NI8CVE4M9UCDMG'})
      #conn.request("POST", "/update", params, headers)
      #response = conn.getresponse()
      #print response.status, response.reason
      #data = response.read()
      #conn.close()

      #urlsms  = "http://freesms4us.com/kirimsms.php?"
          #smsattr = [('user','labtelkom'),('pass','ubayemenz'),('no','087877300496'),('isi',state)]
          #data = urllib.urlencode(smsattr)
          #reqsms = urlsms + data
          #print reqsms
          #reply = urllib2.urlopen(reqsms)
          #print reply.read()

      
      #global state
      if StatR == "3":
          StatR = "Baik"        
      elif StatR == "0":
          StatR = "Rusak 00"
      elif StatR == "1":
          StatR = "Rusak 01"           
      elif StatR == "2":
          StatR = "Rusak 10" 

      if PIR == "1":
          PIR = "Ada Orang"
      else:
          PIR = "Tak ada orang"

      if Stat == "1":
          Stat = "On"
      else:
          Stat = "Off"

      if So == "1":
          So = "PLN"
      else:
          So = "Baterai"

      state = "Hari ini: " + str(i.hour)+":"+str(i.minute)+" " + str(i.day) + "/" + str(i.month) +"/" + str(i.year)+"/" + str(StatR)

      if StatR != "3":
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
          
      
      print state
      print "Lampu:", ID +"| Arus:", I + " A" + "| Tegangan:", V + " V" +"| Kapasitas:", C + "%" +"| On/Off:", Stat + "| Sumber:", So + "| PIR:", PIR + "| Status:", StatR  
               
if __name__ == "__main__":
    while True:
        
        #LDR = RCtime(4)
        #print LDR
        #if LDR >12 and check_time(time_to_check, sore_time, malam_time, pagi_time, siang_time)== 2:
        #  print "gelap"
        #  ser.write("?1\n")
        #  time.sleep(1)
        #else:
        #  print "terang"
        #  ser.write("?0\n")
        #  time.sleep(1) 
        ser.write("1@\n")
        time.sleep(1)
        doit()
        time.sleep(1)
        #ser.close    


