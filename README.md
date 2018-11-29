# 2018-Telstra-Innovation-Challenge

This repository was my team's submission for the 2018 [Telstra Innovation Challenge](https://innovationchallenges.global/challenge/10).

### Team Members

- Andrew Le

- Aiden (Hojin) Lee

### Our Submission

We developed a wearable smart device that automatically detects falls, intending to help monitor people that are highly prone to injuries when falling e.g. senior citizens. 

Our solution utilised a machine learning classifier trained on people's movement via an accelerometer to determine whether a fall had occurred and prevent false alarms in situations when the user is performing similar movements to falling such as bending over, sitting down or going down stairs.

#### Wearable Device
To measure a user's movement, we used an accelerometer attached to a small arduino board, which acted as the wearable device, that was provided by the organisers of the competition. The accelerometer data was then sent to our backend which consisted of Microsofot Azure's IoT Hub and Machine Learning services. Since the majority of a person's daily activity does not typically represent the movement involved in a fall (e.g. sitting, standing and lying down) a dynamic window segmentation method was used to minimise the throughpout of the device.

#### Backend
When a fall-like movement has occurred, the resulting accelerometer data is then sent to the backend where it is passed into Azure's IoT Hub service where features for the classifiers are constructed and various analytics are displayed. The features are then passed into the classifier where it classifies whether the fall-like movement was truly a fall or not alerting the relevant parties if a fall has occurred.

