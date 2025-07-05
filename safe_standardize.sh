#!/bin/bash

# Cẩn thận! Standardize metadata cho tất cả files
# Chỉ thay đổi gid -> id và philosophy_gid -> philosophy

echo "=== CHUẨN HÓA METADATA AN TOÀN ==="

# Backup toàn bộ trước khi bắt đầu
echo "Tạo backup toàn bộ..."
cp -r /home/x/Projects/akao/rules /home/x/Projects/akao/rules_backup_$(date +%Y%m%d_%H%M%S)
cp -r /home/x/Projects/akao/philosophies /home/x/Projects/akao/philosophies_backup_$(date +%Y%m%d_%H%M%S)

# Function để standardize 1 file cẩn thận
safe_standardize() {
    local file="$1"
    local type="$2"
    
    if [ ! -f "$file" ]; then
        echo "⚠️  File không tồn tại: $file"
        return 1
    fi
    
    echo "Processing $type: $(basename $file)"
    
    # Chỉ thay đổi metadata, không động vào nội dung khác
    if grep -q "gid:" "$file"; then
        sed -i 's/  gid:/  id:/g' "$file"
        echo "  ✓ Changed gid -> id"
    fi
    
    if grep -q "philosophy_gid:" "$file"; then
        # Extract category and name for philosophy reference
        local category=$(basename $(dirname "$file"))
        local name=$(basename "$file" .v1.yaml)
        sed -i "s/  philosophy_gid: \"akao:philosophy::.*\"/  philosophy: \"$category\/$name\"/" "$file"
        echo "  ✓ Changed philosophy_gid -> philosophy: $category/$name"
    fi
}

# Process tất cả rule files
echo -e "\n--- Processing RULE files ---"
find /home/x/Projects/akao/rules -name "*.yaml" -type f | while read file; do
    safe_standardize "$file" "RULE"
done

echo -e "\n--- Processing PHILOSOPHY files ---"
find /home/x/Projects/akao/philosophies -name "*.yaml" -type f | grep -v index.yaml | while read file; do
    safe_standardize "$file" "PHILOSOPHY"
done

echo -e "\n=== HOÀN THÀNH AN TOÀN ==="
echo "Backup được tạo với timestamp"
echo "Kiểm tra lại files trước khi commit!"
