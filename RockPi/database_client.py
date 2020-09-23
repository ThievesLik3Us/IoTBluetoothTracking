import pymysql

database_cred_file = open('../certs/database_creds.txt','r')

# Grab the credentials from the document
hostname = database_cred_file.readline()
port_number = database_cred_file.readline()
username = database_cred_file.readline()
password = database_cred_file.readline()
database = database_cred_file.readline()

database_cred_file.close()

#
db_connection = pymysql.connect('p3plnnghwpcdb00-36-a0.prod.phx3.secureserver.net', 'kolbyomalley', '$P$BPlwmmqmDrcW46DU8j5zizB47HGgjm1', 'dzf943576291223')

# prepare a cursor object using cursor() method
cursor = db.cursor()

# execute SQL query using execute() method.
cursor.execute("SELECT * FROM Person")

# Fetch a single row using fetchone() method.
data = cursor.fetchone()
print("Persons : " + data)

# disconnect from server
db.close()
