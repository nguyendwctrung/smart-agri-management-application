import firebase_admin
from firebase_admin import credentials, db

cred = credentials.Certificate("embedded/raspberry_pi/serviceAccountKey.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://smart-agri-management-app-default-rtdb.asia-southeast1.firebasedatabase.app/'
})

ref = db.reference('test_connection')
ref.set({
    'status': 'Online',
    'message': 'Connection success',
    'timestamp': {'.sv': 'timestamp'}
})

print("Sending first data to Firebase...")
print("Successfull! Check Firebase Console")

