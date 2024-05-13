# CS361-Project
Project for CS 361

[Insert information about project here]

# RNG microservice communication contract:

The RNG microservice can generate either a single, or a list of random numbers within a given range. It uses a rest API, and by default it recieves requests on the address http://127.0.0.1:5000. To change the address and port, adjust the ADDRESS and PORT variables in rngService.py.

## Requesting and receiving data: 

To request data from the service, use import the requests library and use the function: 
```request.post(ADDRESS,json=DATA)```

To reviece the data from the service, call .json() on the result of the request.post() call and store the result. For example:
```response = request.post(ADDRESS,json=DATA).json()```
response should contain the output from the service after this is executed.

### Address
The address should be the address and port you set in rngService.py (127.0.0.1 and 5000 by default) in the format address:port, and one of the following paths: /seed, /generateOne, or /generateList. Use ./seed when you want to set the seed for the RNG, use /generateOne when you want to generate one random number, and use /generateList when you want to generate a list of random numbers. Here is an example address:
```127.0.0.1:5000/seed```

### Data
The data you will need to include will depend on what you want the service to do.

/seed: This route will set the seed to a given value. You will need to send the seed you want to set the RNG to. Here is an example call:
```
seed = 10
response = request.post(http://127.0.0.1:5000/seed,json=seed).json()
```
Response will either be "success" or {"error" : "Invalid seed"}

/generateOne: This route will generate one random integer. You will need to send the maximum value(inclusive) you want to generate. You may optionally set the minimum value(inclusive) you want to generate(0 by default). The data should be in the format: {"min": MIN_VAL, "max" : MAX_VAL}. Here is an example call:
```
args = {"min": 1, "max" : 20}
response = request.post(http://127.0.0.1:5000/generateOne,json=args).json()
```
Response will either be a random number or {"error" : ERROR_MESSAGE}

/generateList: This route will generate a list of random integers. You will need to send the maximum value(inclusive) you want to generate and the amount of numbers you want to generate. You may optionally set the minimum value(inclusive) you want to generate(0 by default). The data should be in the format: {"min": MIN_VAL, "max" : MAX_VAL, "amount" : AMOUNT}. Here is an example call:
```
args = {"min": 1, "max" : 20, "amount" : 5}
response = request.post(http://127.0.0.1:5000/generateList,json=args).json()
```
Response will either be list of random numbers or {"error" : ERROR_MESSAGE}

## UML sequence diagram
![UML sequence diagram](/images/UML%20Sequence%20diagram%20for%20RNG%20microservice.png)