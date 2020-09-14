import json
import mysql.connector
from mysql.connector import Error
import hashlib

# Returns a tuple from the validated and parsed JSON pbject
def Parse_JSON_Object(json_object):
    # grab all the values inside the json object
    username = json_object['username']
    email = json_object['email']
    password_clear = json_object['password']

    # Conver the cleartext password to the md5 equivalent - MAYBE add a salt to this
    md5_password = hashlib.md5(bytes(password_clear, 'utf-8'))

    # Print out values to ensure they are correct
    print("username: ", username)
    print("email: ", email)
    print("password: ", password_clear)
    print("md5 password: ", md5_password.hexdigest())

    # checks to username for no spaces
    # check email for @ and . and no spaces
    # check password is at least characters and has letters, numbers, and symbols
    if(not name_letter.isspace() for name_letter in username):
        print("Your username seems legit\n")
        if('@' and "." in email):
            print("Your email seems legit\n")
            if((not pass_char.isspace() for pass_char in password_clear) and (len(password_clear) > 7)):
                print("Your password seems legit\n")
                valid_tuple = (username, email, md5_password.hexdigest())
                return valid_tuple
            else:
                print("Your password is weak\n")
                return -1
        else:
            print("Your email is bullshit\n")
            return -1
    else:
        print("Your username sucks\n")
        return -1

# Connect to the database and return the connection to it
def Connect_To_DB():
    try:
        # Attempt to connect to the DB
        connection = mysql.connector.connect(host='localhost',
                                             database='test_db',
                                             user='test_user1',
                                             password='password123!')
        # Ensure we correctly connected to the DB
        if connection.is_connected():
            # return the connection if so
            return connection

    except mysql.connector.Error as error:
        print("The computer gods refused your connection to the database {}".format(error))
        return -1

# Store a passed in JSON object into the database
def Store_Into_DB(json_object):
    # Create a tuple from the JSON Object
    valid_tuple = Parse_JSON_Object(json_object)

    # Establish a connection to the database
    connection = Connect_To_DB()
    try:
        # Obtain a cursor to the database
        cursor = connection.cursor()

        # Create an SQl query from the tuple values
        insert_query = """INSERT INTO users (username, email, password) VALUES ('{}', '{}', '{}');""".format(valid_tuple[0], valid_tuple[1], valid_tuple[2])
        print(insert_query)

        # Execute the SQL query
        cursor.execute(insert_query)

        # This method sends a COMMIT statement to the MySQL server, committing the current transaction.
        connection.commit()
        print("Successfully inserted into user table")

        # Close out the cursor and connection
        cursor.close()
        connection.close()
        print("DB is closed ... for now\n")
        
    except mysql.connector.Error as error:
        print("Users table is apprently too cool for us {}".format(error))
