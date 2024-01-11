# EyeTracking_Omnicept

Virtual Reality project for Unreal Engine 4.27 for testing the HP Reverb G2 VR Headset and OmniceptSDK. 

## Omnicept SDK

## Setup 
Complete description and images of steps are provided in [HP Developer Portal](https://developers.hp.com/omnicept/docs/fundamentals), I recommend you follow these if it is your first time using OmniceptSDK and HPGlia Plugin. 

### tl;dr
1. Create HP Developer account and then download (and install) [Omnicept Developer SDK](https://developers.hp.com/omnicept/hp-omnicept-sdk).
2. Follow [these steps](https://developers.hp.com/omnicept/docs/console/getting-started) to generate an HP developer license (composed of an client id and access key) in the [HP Console](https://omnicept-console.hpbp.io/).
3. Follow [usage steps](https://developers.hp.com/omnicept/docs/ue4/getting-started#usage) to attach license (remember it's 2 items) values into ```HPGlia Settings ``` from within the Unreal Editor (```Edit > Project Settings > Plugins > HP Glia Settings```).

#### IMPORTANT SECURITY NOTE 
Taken from [Unreal > Getting started](https://developers.hp.com/omnicept/docs/ue4/getting-started): 
```
To prevent Client ID and Access Key values from being readable in a packaged project, enable encryption for initialization
files. Under Edit -> Project Settings -> Project -> Crypto click "Generate New Encryption Key" button, then check the box
for "Encrypt Pak Ini Files".
```
Read more [here](https://developers.hp.com/omnicept/docs/ue4/getting-started#securing).

