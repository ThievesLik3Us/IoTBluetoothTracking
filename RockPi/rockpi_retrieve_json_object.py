import json
import mysql.connector
from mysql.connector import Error
import hashlib

# This function is so we can perform additional actions on the data before sending it as a JSON object
def Construct_JSON_Object(retrieved_tuple):
    # maybe do some parsing and error checking
    # create json_object format
    json_object =  { "username":"", "email":"", "password":"" }

    # Convert the tuple to a JSON object
    json_object['username'] = json.dumps(retrieved_tuple[0])
    json_object['email'] = json.dumps(retrieved_tuple[1])
    json_object['password'] = json.dumps(retrieved_tuple[2])

    print(json_object)

    # Change the username just for these tests
    json_object['username'] = "myuser1"

    # return the JSON object
    return json_object

# Retrieves the tuple from the database with the passed in value
def Retrieve_From_DB(table_name, username):
    try:
        # Connect to the DB
        connection = mysql.connector.connect(host='localhost',
                                             database='test_db',
                                             user='test_user1',
                                             password='password123!')
        # Ensure we corrently connected
        if connection.is_connected():
            # SQL statement to select all users with the passed in username
            retrieve_query = """SELECT * FROM {} WHERE username='{}';""".format(table_name, username)
            print(retrieve_query)

            # Establish conenction cursor
            cursor = connection.cursor()

            # Execute SQL statement
            cursor.execute(retrieve_query)

            # Fetch the one user generated from the SQL statement
            retrieved_tuple = cursor.fetchone()

            # close the DB connection
            cursor.close()
            connection.close()

            # See if we retrieved the value correctly or no value was returned
            if(retrieved_tuple):
                print("Successfully retrieved {} from user table".format(retrieved_tuple))
            else:
                print("It seems that user doesn't exist")
                return -1

            # Create a JSON object from the tuple
            json_object = Construct_JSON_Object(retrieved_tuple)

            # return the JSON object
            return json_object

    except mysql.connector.Error as error:
        print("The computer gods refused your connection to the database {}".format(error))
        return -1
