# -*- coding: windows-1254 -*-
"""
Tüm .py dosyalarını Windows-1254 encoding'e dönüştürür
"""
import os
import sys

def convert_file_to_windows1254(filepath):
    """Bir dosyayı Windows-1254 encoding'e dönüştürür"""
    try:
        # Önce dosyayı oku (farklı encoding'leri dene)
        content = None
        encodings_to_try = ['utf-8', 'utf-8-sig', 'latin-1', 'cp1254', 'windows-1254', 'iso-8859-9']
        
        for enc in encodings_to_try:
            try:
                with open(filepath, 'r', encoding=enc) as f:
                    content = f.read()
                break
            except (UnicodeDecodeError, UnicodeError):
                continue
        
        if content is None:
            print("HATA: %s dosyası okunamadı (encoding tespit edilemedi)" % filepath)
            return False
        
        # Encoding declaration kontrolü
        has_encoding_decl = False
        lines = content.split('\n')
        
        # İlk 2 satırda encoding declaration var mı kontrol et
        for i, line in enumerate(lines[:2]):
            if 'coding:' in line or 'encoding:' in line:
                has_encoding_decl = True
                # Mevcut encoding declaration'ı güncelle
                if 'coding:' in line:
                    lines[i] = '# -*- coding: windows-1254 -*-'
                elif 'encoding:' in line:
                    lines[i] = '# -*- coding: windows-1254 -*-'
                break
        
        # Eğer encoding declaration yoksa, dosya başına ekle
        if not has_encoding_decl:
            # Shebang varsa ondan sonra, yoksa en başa ekle
            if lines and lines[0].startswith('#!'):
                lines.insert(1, '# -*- coding: windows-1254 -*-')
            else:
                lines.insert(0, '# -*- coding: windows-1254 -*-')
        
        content = '\n'.join(lines)
        
        # Windows-1254 encoding ile kaydet (BOM olmadan)
        with open(filepath, 'w', encoding='windows-1254', errors='replace') as f:
            f.write(content)
        
        return True
        
    except Exception as e:
        print("HATA: %s dosyası işlenirken hata: %s" % (filepath, str(e)))
        return False

def main():
    """Ana fonksiyon"""
    # Script'in bulunduğu dizin
    root_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Tüm .py dosyalarını bul (recursive)
    py_files = []
    for root, dirs, files in os.walk(root_dir):
        # Script'in kendisini hariç tut
        for file in files:
            if file.endswith('.py') and file != 'convert_encoding.py':
                py_files.append(os.path.join(root, file))
    
    print("Toplam %d .py dosyası bulundu" % len(py_files))
    print("Encoding dönüşümü başlatılıyor...\n")
    
    success_count = 0
    fail_count = 0
    
    for py_file in py_files:
        rel_path = os.path.relpath(py_file, root_dir)
        print("İşleniyor: %s" % rel_path)
        
        if convert_file_to_windows1254(py_file):
            success_count += 1
        else:
            fail_count += 1
    
    print("\n" + "="*50)
    print("Dönüşüm tamamlandı!")
    print("Başarılı: %d" % success_count)
    print("Başarısız: %d" % fail_count)
    print("="*50)

if __name__ == '__main__':
    main()

