import json
import mysql.connector
from mysql.connector import Error
import hashlib


def Parse_JSON_Object(json_object):
    # grab all the values inside the json object
    username = json_object['username']
    email = json_object['email']
    password_clear = json_object['password']
    print("username: ", username)
    print("email: ", email)
    print("password: ", password_clear)
    md5_password = hashlib.md5(bytes(password_clear, 'utf-8'))
    print("md5 password: ", md5_password.digest())

    # checks to username for no spaces
    # check email for @ and . and no spaces
    # check password is at least characters and has letters, numbers, and symbols
    if(not name_letter.isspace() for name_letter in username):
        print("Your username seems legit\n")
        if('@' and "." in email):
            print("Your email seems legit\n")
            if((not pass_char.isspace() for pass_char in password_clear) and (len(password_clear) > 7)):
                print("Your password seems legit\n")
                valid_tuple = (username, email, md5_password.digest())
                return valid_tuple
            else:
                print("Your password is bullshit\n")
                return -1
        else:
            print("Your email is bullshit\n")
            return -1
    else:
        print("Your username sucks\n")
        return -1


def Connect_To_DB():
    try:
        connection = mysql.connector.connect(host='localhost',
                                             database='test_db',
                                             user='test_user1',
                                             password='password123!')
        if connection.is_connected():
            db_Info = connection.get_server_info()
            print("Connected to MySQL Server version ", db_Info)
            cursor = connection.cursor()
            cursor.execute("select database();")
            record = cursor.fetchone()
            print("You're connected to database: ", record)
            cursor.close()
            return connection

    except mysql.connector.Error as error:
        print("The computer gods refused your connection to the database {}".format(error))
        return -1

def Store_into_DB(json_object):
    valid_tuple = Parse_JSON_Object(json_object)
    connection = Connect_To_DB()
    try:
        cursor = connection.cursor()
        insert_query = """INSERT INTO users (username, email, password)
                                    VALUES (%s, %s, %s) """
        cursor.execute(insert_query, valid_tuple)
        connection.commit()
        print("Successfully inserted into user table")
        cursor.close()
    except mysql.connector.Error as error:
        print("Users table is apprently too cool for us {}".format(error))
    finally:
        connection.close()
        print("DB is closed ... for now\n")
