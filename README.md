# Overview

The software I built using C++ is a financial transaction authorizer, it uses classes to read JSON files with transaction data, iterate through them, process and validate transaction information such as card, expiration date, CVV, amount and transaction id, then it writes to a log file a successful or failure transaction message. I have developed this software to further my knowledge using c++, json libraries and classes to build a great app.

[Software Demo Video](http://youtube.link.goes.here)

# Development Environment

I used c++ language, JSON library (RapidJSON), ctime and vector data structure from STL.
I used the MinGW Gnu Compiler Collection (GCC).

You must set FTM_HOME environmental variable to run the program.

Windows example commands
-$env:FTM_HOME = "C:\projects\CSE310_C++\ftm_authorizer_cpp"
-echo $env:FTM_HOME

# Useful Websites

- [w3schools](https://www.w3schools.com/cpp/)
- [simplilearn](https://www.simplilearn.com/tutorials/cpp-tutorial/)
- [geeksforgeeks](https://www.geeksforgeeks.org/how-to-read-and-parse-json-file-with-rapidjson/)

# Future Work

- Add more transaction data validation
- Add a database with accounts, credit limits, log of transactions, etc.
- Use ISO8583 transaction format
