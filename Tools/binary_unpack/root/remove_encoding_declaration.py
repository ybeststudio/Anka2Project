# -*- coding: windows-1254 -*-
"""
Tüm .py dosyalarından encoding declaration satırlarını kaldırır
"""
import os
import sys

def remove_encoding_declaration(filepath):
    """Bir dosyadan encoding declaration satırını kaldırır"""
    try:
        # Dosyayı oku
        with open(filepath, 'r', encoding='windows-1254') as f:
            lines = f.readlines()
        
        # Encoding declaration satırlarını bul ve kaldır
        new_lines = []
        removed = False
        
        for i, line in enumerate(lines):
            # Encoding declaration kontrolü
            stripped = line.strip()
            if ('# -*- coding:' in stripped or 
                '# -*- encoding:' in stripped or
                '# coding:' in stripped or
                '# encoding:' in stripped or
                '# vim: set fileencoding=' in stripped):
                removed = True
                continue  # Bu satırı atla
            
            new_lines.append(line)
        
        # Eğer bir satır kaldırıldıysa dosyayı yeniden yaz
        if removed:
            with open(filepath, 'w', encoding='windows-1254') as f:
                f.writelines(new_lines)
            return True
        
        return False
        
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
            if file.endswith('.py') and file != 'remove_encoding_declaration.py' and file != 'convert_encoding.py':
                py_files.append(os.path.join(root, file))
    
    print("Toplam %d .py dosyası bulundu" % len(py_files))
    print("Encoding declaration satırları kaldırılıyor...\n")
    
    success_count = 0
    removed_count = 0
    fail_count = 0
    
    for py_file in py_files:
        rel_path = os.path.relpath(py_file, root_dir)
        
        if remove_encoding_declaration(py_file):
            print("Kaldırıldı: %s" % rel_path)
            removed_count += 1
            success_count += 1
        else:
            success_count += 1
    
    print("\n" + "="*50)
    print("İşlem tamamlandı!")
    print("Toplam dosya: %d" % len(py_files))
    print("Encoding declaration kaldırılan: %d" % removed_count)
    print("Değişiklik olmayan: %d" % (success_count - removed_count))
    print("="*50)

if __name__ == '__main__':
    main()

