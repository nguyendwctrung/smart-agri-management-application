import psycopg2
import os
from dotenv import load_dotenv

# Load biến môi trường từ file .env
load_dotenv()

try:
    conn = psycopg2.connect(
        host=os.getenv("PGHOST"),
        database=os.getenv("PGDATABASE"),
        user=os.getenv("PGUSER"),
        password=os.getenv("PGPASSWORD"),
        sslmode='require'
    )
    print("Kết nối Neon thành công qua biến môi trường!")
    
    # Đừng quên đóng kết nối
    conn.close()
    
except Exception as e:
    print(f"Lỗi kết nối: {e}")