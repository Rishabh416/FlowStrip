import cv2
import numpy as np

# preprocess the image
cap = cv2.VideoCapture(0)

while cap.isOpened(): 
    ret, frame = cap.read()
    if not ret: 
        break
    
    points = []

    # convert frame to HSV
    hsvFrame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # range of blue colors to detect
    lowerBlue = np.array([100, 100, 100])
    upperBlue = np.array([130, 255, 255])

    # create a mask given the range of colors and apply it on the frame
    mask = cv2.inRange(hsvFrame, lowerBlue, upperBlue)
    filtered = cv2.bitwise_and(frame, frame, mask=mask)

    # get the centers of the segments/blobs
    _, _, _, centroids = cv2.connectedComponentsWithStats(mask, 8)

    for center in centroids[1:]:
        point = tuple(np.round(center).astype(int))
        points.append(point)
        cv2.circle(filtered, point, 5, (0, 255, 0), -1)
        cv2.putText(filtered, f"({point[0]}, {point[1]})", (point[0] + 8, point[1]),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)

    print(points)

    cv2.imshow("Filtered Image", filtered)
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break


cap.release()
cv2.destroyAllWindows()