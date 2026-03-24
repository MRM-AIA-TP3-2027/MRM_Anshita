import cv2
import numpy as np

# Open webcam
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Camera not working")
    exit()

# Marker size in meters (change if your marker size is different)
marker_length = 0.05

# Camera calibration approximation
camera_matrix = np.array([
    [800, 0, 320],
    [0, 800, 240],
    [0, 0, 1]
], dtype=float)

dist_coeffs = np.zeros((5,1))

# ArUco dictionaries required for assignment
dictionaries = [
    cv2.aruco.DICT_4X4_50,
    cv2.aruco.DICT_5X5_50,
    cv2.aruco.DICT_6X6_50
]

parameters = cv2.aruco.DetectorParameters()

while True:

    ret, frame = cap.read()

    if not ret:
        print("Camera not working")
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Try detecting markers from each dictionary
    for dict_type in dictionaries:

        aruco_dict = cv2.aruco.getPredefinedDictionary(dict_type)

        corners, ids, rejected = cv2.aruco.detectMarkers(
            gray,
            aruco_dict,
            parameters=parameters
        )

        if ids is not None:

            # Draw marker boundaries
            cv2.aruco.drawDetectedMarkers(frame, corners, ids)

            # Estimate pose
            rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(
                corners,
                marker_length,
                camera_matrix,
                dist_coeffs
            )

            for i in range(len(ids)):

                rvec = rvecs[i][0]
                tvec = tvecs[i][0]

                cv2.putText(frame, f"rvec: {rvec[0]:.2f}, {rvec[1]:.2f}, {rvec[2]:.2f}",
                            (10, 70 + i*30),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.6,
                            (255,0,0),
                            2)

                cv2.putText(frame, f"tvec: {tvec[0]:.2f}, {tvec[1]:.2f}, {tvec[2]:.2f}",
                            (10, 100 + i*30),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.6,
                            (0,255,255),
                            2)

                # Draw coordinate axes
                cv2.drawFrameAxes(
                    frame,
                    camera_matrix,
                    dist_coeffs,
                    rvecs[i],
                    tvecs[i],
                    0.03
                )

                # Get depth from translation vector
                distance = tvecs[i][0][2]

                # Display ID and distance
                cv2.putText(
                    frame,
                    f"ID: {ids[i][0]} Dist: {distance:.2f}m",
                    (10, 40 + i*30),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.8,
                    (0,255,0),
                    2
                )

    cv2.imshow("ArUco Pose Estimation", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()