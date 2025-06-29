#coding: cp932

import openpyxl
from datetime import datetime

# ワークブックとシートの作成
workbook = openpyxl.Workbook()
sheet = workbook.active

sheet['B2']='請求書'
sheet['B4']='株式会社ABC'
sheet['B5']='〒101-0022　東京都千代田区神田練塀町300'
sheet['B6']='TEL：03-1234-5678　FAX：-03-1234-5678'
sheet['B7']='担当者名：鈴木一郎　様'
sheet['B10']='商品名'
sheet['F4']='No.'
sheet['F5']='日付'

sheet['G4']='0001'

# 日付の設定
today = datetime.today()
sheet['G5']=today.strftime('%Y/%m/%d')
sheet['C9']='2'
sheet['C10']='=1+1'
sheet.cell(row=1, column=1).value = '=1+1'
sheet['D10']='単価'
sheet['E10']='金額'

#商品データ
product = [
    ['商品A', 2, 10000],
    ['商品B', 1, 15000],
]

#商品表示
row = 11        #入力行
total = 0       #合計
for name, quantity, value in product:
    sheet.cell(row=row, column=2, value=name)
    sheet.cell(row=row, column=3, value=quantity)
    sheet.cell(row=row, column=4, value=value)
    sheet.cell(row=row, column=5, value=quantity*value)
    total += quantity*value
    row += 1
sheet.cell(row=row, column=5, value=total)

#合計計算
row += 2
summary = [
    ['小計', total],
    ['消費税', total*0.1],
    ['合計', total*1.1]
]
#合計表示
for s, v in summary:
    sheet.cell(row=row, column=2, value=s)
    sheet.cell(row=row, column=5, value=v)
    row += 1

fn='請求書_' + today.strftime('%Y%m%d') + '.xlsx'
workbook.save(fn)

