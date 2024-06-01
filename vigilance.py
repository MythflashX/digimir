import mediapipe as mp
import cv2
import numpy as np
import math

''' - se[t]up - '''
feed = cv2.VideoCapture(0)              # capture images from webcam 0
mp_hands = mp.solutions.hands           # hand detection and tracking
mp_drawing = mp.solutions.drawing_utils # drawing detected hand landmarks

''' - fun[c]tions - '''
def calc_dis(poiA, poiB):
    x1 = poiA.x; y1 = poiA.y
    x2 = poiB.x; y2 = poiB.y
    return math.sqrt((x2 - x1)**2 + (y2 - y1)**2)       # Euclidean distance formula (calculate distance from 2 points)

def calc_prcntge(min, max, val):                # Convert finger's angle into value between 0 and 100 percent for exporting
    percentage = int(((val-max) / (min - max))* 100)
    if percentage > 0 & percentage < 100:
        return percentage
    else:
        return 0

def prep_distances(landmarks):                                       # distance from each finger tip to base of palm
    dis_fngrs = []
    dis_fngrs.append(int(calc_prcntge(0.21, 0.37, calc_dis(landmarks[4], landmarks[0]))))                # those float are min and max distance from landmark[i] to landmark[0]
    dis_fngrs.append(int(calc_prcntge(0.16, 0.60, calc_dis(landmarks[8], landmarks[0]))))                             # they are constants now
    dis_fngrs.append(int(calc_prcntge(0.13, 0.63, calc_dis(landmarks[12], landmarks[0]))))                            # fix them to get dynamic max & min distance at the start of live feed
    dis_fngrs.append(int(calc_prcntge(0.13, 0.57, calc_dis(landmarks[16], landmarks[0]))))
    dis_fngrs.append(int(calc_prcntge(0.13, 0.50, calc_dis(landmarks[20], landmarks[0]))))
    return dis_fngrs

def export(dis_fngrs):
    message = ','.join(str(fngr) for fngr in dis_fngrs)
    with open('convergence.txt', 'w') as file:
        file.write(message)

''' - ma[i]n lo[o]p - '''
with mp_hands.Hands(min_detection_confidence=0.5, min_tracking_confidence=0.5) as hands:  # create an instance(object [hands]) of the mp_hands.Hands class with arguments provided..."with" which is a context manager do all the nitty gritty work of handling the class properly
    while feed.isOpened():  # check if the video feed is live
        success, frame = feed.read()     # extract status of the video feed and current(latest) frame
        if not success:                  # if the video feed isn't live restart the loop
            print("Empty Camera Frame")
            continue

        img = frame.copy()               # prevent altering the original frame
        results = hands.process(img)     # detect & track hand landmarks
        if results.multi_hand_landmarks:    # if hand was detected in the frame
            for hand_landmarks in results.multi_hand_landmarks:  # extract the hand landmarks
                mp_drawing.draw_landmarks(img, hand_landmarks, mp_hands.HAND_CONNECTIONS) # draw the hand skeleton on detected hand
                export(prep_distances(hand_landmarks.landmark))

        cv2.imshow("Vigilance", img)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
feed.release()

