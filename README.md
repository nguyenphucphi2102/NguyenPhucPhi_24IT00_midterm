# Lệnh `ls` của UNIX - Phiên bản đơn giản hóa

Bài thi giữa kỳ: lập trình lại lệnh `ls(1)` bằng ngôn ngữ C, theo trang manual `ls(1)` của NetBSD 10.1. Dự án giúp hiểu các thao tác trên hệ thống tập tin UNIX và lập trình ở mức hệ thống.

## Thông tin sinh viên

- **Sinh viên:** Nguyễn Phúc Phi
- **MSSV:** 24IT200
- **Môn học:** [ĐIỀN TÊN MÔN HỌC]
- **GitHub:** https://github.com/nguyenphucphi2102/NguyenPhucPhi_24IT200_midterm

## Môi trường phát triển

- Hệ điều hành: NetBSD 10.1 (amd64), chạy trong máy ảo VirtualBox, truy cập bằng SSH
- Trình biên dịch: `cc` (GCC 10.5.0)
- Công cụ build: `make` (BSD make của NetBSD; Makefile cũng chạy được với GNU make)
- Quản lý mã nguồn: Git

## Biên dịch và chạy

    git clone git@github.com:nguyenphucphi2102/NguyenPhucPhi_24IT200_midterm.git
    cd NguyenPhucPhi_24IT200_midterm
    make            # tạo file thực thi ./myls
    ./myls          # liệt kê thư mục hiện tại
    make clean      # xóa file thực thi và các file .o

Chương trình có tên `myls` để không trùng với `/bin/ls` của hệ thống. Phải gõ `./myls` (có `./`) khi chạy.

## Cấu trúc dự án

    .
    ├── Makefile      # biên dịch dự án
    ├── README.md     # báo cáo này
    ├── .gitignore    # loại trừ file thực thi và file .o
    ├── ls.h          # struct options: toàn bộ cờ dòng lệnh
    ├── main.c        # xử lý operand, đệ quy -R, điều khiển luồng chính
    ├── options.c/.h  # phân tích option bằng getopt
    ├── entry.c/.h    # danh sách entry, stat/lstat, đọc thư mục
    ├── sort.c/.h     # sắp xếp: tên, -t, -S, -r, -f
    ├── print.c/.h    # in kết quả: -i -s -F -q -w -l, dòng total
    ├── format.c/.h   # chọn loại thời gian, BLOCKSIZE, kích thước dễ đọc, ký hiệu -F
    ├── longfmt.c/.h  # định dạng dài: mode, ngày giờ, tên owner/group
    └── util.c/.h     # cấp phát bộ nhớ an toàn, nối đường dẫn, báo lỗi

Chương trình tự cài đặt mọi chức năng, không dùng `strmode(3)`, `humanize_number(3)` hay `getbsize(3)`.

## Các tính năng đã cài đặt

Hỗ trợ đủ các option trong trang manual được cung cấp: `-AacdFfhiklnqRrSstuw`.

| Option | Chức năng |
|--------|-----------|
| `-A` | Liệt kê mọi entry trừ `.` và `..`. Luôn bật khi chạy bằng root |
| `-a` | Bao gồm cả entry bắt đầu bằng dấu chấm |
| `-c` | Dùng thời gian đổi trạng thái (ctime) để sắp xếp (`-t`) hoặc in (`-l`) |
| `-d` | Thư mục được liệt kê như file thường; symlink ở operand không bị đi xuyên qua |
| `-F` | Thêm ký hiệu: `/` thư mục, `*` thực thi, `@` symlink, `%` whiteout, `=` socket, `\|` FIFO |
| `-f` | Không sắp xếp |
| `-h` | Kích thước dạng dễ đọc (B, K, M, G...) cho `-s` và `-l`. Ghi đè `-k` |
| `-i` | In số inode |
| `-k` | Số block tính theo đơn vị kilobyte. Ghi đè `-h` |
| `-l` | Định dạng dài (mode, số link, owner, group, kích thước, thời gian, tên) |
| `-n` | Như `-l` nhưng owner và group hiển thị dạng số |
| `-q` | In `?` thay cho ký tự không in được (mặc định khi xuất ra terminal) |
| `-R` | Liệt kê đệ quy thư mục con (không đi theo symlink) |
| `-r` | Đảo ngược thứ tự sắp xếp |
| `-S` | Sắp xếp theo kích thước, lớn nhất trước |
| `-s` | Hiển thị số block đã dùng của mỗi file |
| `-t` | Sắp xếp theo thời gian sửa đổi, mới nhất trước |
| `-u` | Dùng thời gian truy cập (atime) thay cho thời gian sửa đổi |
| `-w` | In nguyên dạng ký tự không in được (mặc định khi xuất ra pipe hoặc file) |

### Hành vi theo trang manual

- Không có operand: liệt kê thư mục hiện tại. Nhiều operand: các operand không phải thư mục in trước, sau đó đến các thư mục; mỗi nhóm sắp xếp riêng theo thứ tự từ điển.
- Các cặp option ghi đè nhau, cái đứng sau thắng: `-l`/`-n`, `-c`/`-u`, `-q`/`-w`, `-R`/`-d`, `-h`/`-k`.
- Định dạng dài hiển thị thêm dòng `total` cho mỗi thư mục. Symlink hiển thị dạng `tên -> đích`. File thiết bị hiển thị `major, minor` ở cột kích thước.
- Cột thời gian: hiển thị `Tháng Ngày Giờ:Phút` với file trong vòng sáu tháng, ngược lại hiển thị `Tháng Ngày Năm`.
- Biến môi trường `BLOCKSIZE` được dùng để tính số block (khi không có `-h` và `-k`); `TZ` ảnh hưởng đến cách hiển thị ngày giờ.
- Mã thoát là 0 nếu thành công, lớn hơn 0 nếu có lỗi.

## Ví dụ sử dụng

    ./myls                      # thư mục hiện tại
    ./myls -l                   # định dạng dài
    ./myls -la                  # dạng dài, gồm cả file ẩn
    ./myls -lh /etc             # kích thước dễ đọc
    ./myls -ltr                 # theo thời gian sửa đổi, cũ nhất trước
    ./myls -lS                  # theo kích thước, lớn nhất trước
    ./myls -ltu                 # sắp xếp theo thời gian truy cập
    ./myls -R /etc              # đệ quy
    ./myls -ld /etc /tmp        # liệt kê thư mục như file
    ./myls -siF                 # inode, block, ký hiệu loại file
    ./myls file /tmp /etc       # file in trước, thư mục in sau
    ./myls /nonexistent         # báo lỗi, mã thoát lớn hơn 0

## Thiết kế

- **Modular:** mỗi module làm một việc (phân tích option, đọc thư mục, sắp xếp, in, định dạng) và có file header riêng.
- **Sắp xếp:** dùng `qsort()` với các hàm so sánh theo tên, thời gian (chọn bởi `-c`/`-u`), kích thước. Khi bằng nhau thì so sánh tên.
- **Bộ nhớ:** danh sách entry là mảng tự mở rộng; mọi cấp phát đều qua `xmalloc`/`xrealloc` (thoát khi hết bộ nhớ), và được giải phóng trước khi kết thúc.
- **Căn cột:** mỗi thư mục được quét hai lượt: lượt đầu tính độ rộng lớn nhất của từng cột, lượt hai mới in.
- **Lỗi:** mọi lỗi được in ra stderr dạng `ls: <đường dẫn>: <lý do>`, chương trình tiếp tục xử lý các mục còn lại và trả mã thoát 1 ở cuối.

## Kiểm thử

Chương trình được so sánh với `ls` của chính NetBSD 10.1 trên máy ảo, bằng các script shell: mỗi phép thử chạy cả `myls` và `ls -1` với cùng đối số rồi so sánh nội dung xuất ra và mã thoát. Các nhóm đã thử:

- Từng option đơn và các tổ hợp (`-la`, `-lh`, `-lt`, `-lS`, `-lsi`, `-Rl`...).
- Các cặp option ghi đè nhau theo cả hai thứ tự.
- Sắp xếp với thời gian và kích thước trùng nhau.
- Số block với các giá trị `BLOCKSIZE` khác nhau, kích thước dễ đọc ở các mốc 999, 1000, 1023, 1024 byte.
- Định dạng dài: các bit setuid, setgid, sticky; symlink (kể cả symlink hỏng); FIFO; file thiết bị; hard link; file nằm sát mốc sáu tháng và file có thời gian ở tương lai.
- Đệ quy với thư mục lồng nhau, thư mục ẩn, thư mục rỗng, symlink tới thư mục, thư mục không có quyền đọc.
- Trường hợp biên: tên file có dấu cách, tab, xuống dòng, ký tự điều khiển, byte không phải ASCII, tên bắt đầu bằng `-`, tên dài 250 ký tự, thư mục chứa 2000 file, đường dẫn sâu 40 cấp, operand không tồn tại, `--`, option sai.

Không có phép thử nào làm chương trình bị crash. Chương trình cũng được biên dịch với AddressSanitizer và UndefinedBehaviorSanitizer và chạy trên các thư mục thử; không phát hiện lỗi bộ nhớ.

## Các điểm khác với `ls` thật

Những chỗ trang manual và `ls` thật của NetBSD không thống nhất, hoặc manual không nói rõ:

- **`-R` và `-d`:** theo manual, hai option ghi đè nhau và cái đứng sau thắng. Với `-dR` thì `-R` thắng. `ls` thật của NetBSD cho `-d` thắng bất kể thứ tự.
- **`-f`:** manual chỉ ghi "không sắp xếp", nhưng `ls` thật còn bật cả `-a`; chương trình làm giống `ls` thật.
- **`-s` khi xuất ra terminal:** theo manual, `myls` in dòng `total` trước danh sách. `ls` thật của NetBSD không in dòng này với `-s`. Khi xuất ra pipe hoặc file thì cả hai đều không in.
- **`-S` và `-t`:** nếu dùng cả hai, option đứng sau thắng (giống `ls` thật).
- **Operand là chuỗi rỗng:** `myls` báo lỗi cho operand đó và vẫn liệt kê các operand còn lại; `ls` thật không in gì cả.
- **Định dạng ngắn:** luôn mỗi entry một dòng (theo manual), không chia cột.

## Hạn chế

- Không hỗ trợ output màu hoặc các option ngoài danh sách trong manual (ví dụ `--color`).
- Không hỗ trợ locale đa ngôn ngữ cho tên tháng; dùng theo cấu hình hệ thống.

## Tài liệu tham khảo

- Trang manual `ls(1)`, NetBSD 10.1 (27/10/2024), file `ls.pdf` do giảng viên cung cấp.
- Các trang manual `stat(2)`, `getopt(3)`, `opendir(3)`, `readdir(3)`, `getpwuid(3)`, `getgrgid(3)`, `strftime(3)`.

*Cập nhật lần cuối: tháng 10 năm 2026.*
