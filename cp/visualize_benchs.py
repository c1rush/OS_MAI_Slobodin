import pandas as pd
import matplotlib.pyplot as plt
import os

# Функция для проверки наличия файлов
def check_files_exist(file_paths):
    for file in file_paths:
        if not os.path.isfile(file):
            raise FileNotFoundError(f"Файл {file} не найден в текущей директории.")

# Функция для чтения CSV-файлов
def read_csv_files(file_paths):
    dataframes = {}
    for file in file_paths:
        try:
            df = pd.read_csv(file)
            dataframes[file] = df
        except Exception as e:
            raise IOError(f"Не удалось прочитать файл {file}: {e}")
    return dataframes

# Функции для визуализации каждой метрики
def plot_benchmark_three_internal_frag(df):
    plt.figure(figsize=(10, 6))
    block_df = df[df['AllocatorType'] == 'BlockAllocator']
    freelist_df = df[df['AllocatorType'] == 'FreeListAllocator']
    
    plt.plot(block_df['Iteration'], block_df['InternalFragmentation_bytes'], label='BlockAllocator', color='blue')
    plt.plot(freelist_df['Iteration'], freelist_df['InternalFragmentation_bytes'], label='FreeListAllocator', color='orange')
    plt.title('Внутренняя фрагментация по итерациям')
    plt.xlabel('Итерация')
    plt.ylabel('Internal Fragmentation (bytes)')
    plt.legend()
    plt.tight_layout()
    plt.savefig('benchmark_three_internal_frag.png', bbox_inches='tight')
    plt.show()

def plot_benchmark_three_external_frag(df):
    plt.figure(figsize=(10, 6))
    block_df = df[df['AllocatorType'] == 'BlockAllocator']
    freelist_df = df[df['AllocatorType'] == 'FreeListAllocator']
    
    plt.plot(block_df['Iteration'], block_df['ExternalFragmentation_bytes'], label='BlockAllocator', color='blue')
    plt.plot(freelist_df['Iteration'], freelist_df['ExternalFragmentation_bytes'], label='FreeListAllocator', color='orange')
    plt.title('Внешняя фрагментация по итерациям')
    plt.xlabel('Итерация')
    plt.ylabel('External Fragmentation (bytes)')
    plt.legend()
    plt.tight_layout()
    plt.savefig('benchmark_three_external_frag.png', bbox_inches='tight')
    plt.show()

def plot_benchmark_three_usage_factor(df):
    plt.figure(figsize=(10, 6))
    block_df = df[df['AllocatorType'] == 'BlockAllocator']
    freelist_df = df[df['AllocatorType'] == 'FreeListAllocator']
    
    plt.plot(block_df['Iteration'], block_df['UsageFactor_percent'], label='BlockAllocator', color='blue')
    plt.plot(freelist_df['Iteration'], freelist_df['UsageFactor_percent'], label='FreeListAllocator', color='orange')
    plt.title('Фактор использования по итерациям')
    plt.xlabel('Итерация')
    plt.ylabel('Usage Factor (%)')
    plt.legend()
    plt.tight_layout()
    plt.savefig('benchmark_three_usage_factor.png', bbox_inches='tight')
    plt.show()

def plot_benchmark_four_total_alloc_time(df):
    plt.figure(figsize=(10, 6))
    block_df = df[df['AllocatorType'] == 'BlockAllocator']
    freelist_df = df[df['AllocatorType'] == 'FreeListAllocator']
    
    plt.plot(block_df['AllocationCount'], block_df['TotalAllocTime_us'], label='BlockAllocator', color='blue')
    plt.plot(freelist_df['AllocationCount'], freelist_df['TotalAllocTime_us'], label='FreeListAllocator', color='orange')
    plt.title('Общее время аллокаций по количеству аллокаций')
    plt.xlabel('Количество аллокаций')
    plt.ylabel('Total Alloc Time (µs)')
    plt.legend()
    plt.tight_layout()
    plt.savefig('benchmark_four_total_alloc_time.png', bbox_inches='tight')
    plt.show()

def plot_benchmark_four_avg_alloc_time(df):
    plt.figure(figsize=(10, 6))
    block_df = df[df['AllocatorType'] == 'BlockAllocator']
    freelist_df = df[df['AllocatorType'] == 'FreeListAllocator']
    
    plt.plot(block_df['AllocationCount'], block_df['AvgAllocTime_us'], label='BlockAllocator', color='blue')
    plt.plot(freelist_df['AllocationCount'], freelist_df['AvgAllocTime_us'], label='FreeListAllocator', color='orange')
    plt.title('Среднее время аллокации по количеству аллокаций')
    plt.xlabel('Количество аллокаций')
    plt.ylabel('Avg Alloc Time (µs)')
    plt.legend()
    plt.tight_layout()
    plt.savefig('benchmark_four_avg_alloc_time.png', bbox_inches='tight')
    plt.show()

def plot_benchmark_five_total_free_time(df):
    plt.figure(figsize=(10, 6))
    block_df = df[df['AllocatorType'] == 'BlockAllocator']
    freelist_df = df[df['AllocatorType'] == 'FreeListAllocator']
    
    plt.plot(block_df['DeallocationCount'], block_df['TotalFreeTime_us'], label='BlockAllocator', color='blue')
    plt.plot(freelist_df['DeallocationCount'], freelist_df['TotalFreeTime_us'], label='FreeListAllocator', color='orange')
    plt.title('Общее время деаллокаций по количеству деаллокаций')
    plt.xlabel('Количество деаллокаций')
    plt.ylabel('Total Free Time (µs)')
    plt.legend()
    plt.tight_layout()
    plt.savefig('benchmark_five_total_free_time.png', bbox_inches='tight')
    plt.show()

def plot_benchmark_five_avg_free_time(df):
    plt.figure(figsize=(10, 6))
    block_df = df[df['AllocatorType'] == 'BlockAllocator']
    freelist_df = df[df['AllocatorType'] == 'FreeListAllocator']
    
    plt.plot(block_df['DeallocationCount'], block_df['AvgFreeTime_us'], label='BlockAllocator', color='blue')
    plt.plot(freelist_df['DeallocationCount'], freelist_df['AvgFreeTime_us'], label='FreeListAllocator', color='orange')
    plt.title('Среднее время деаллокации по количеству деаллокаций')
    plt.xlabel('Количество деаллокаций')
    plt.ylabel('Avg Free Time (µs)')
    plt.legend()
    plt.tight_layout()
    plt.savefig('benchmark_five_avg_free_time.png', bbox_inches='tight')
    plt.show()

def main():
    # Путь к CSV-файлам
    csv_files = [
        '/Users/c1rush/OS_MAI_Slobodin/build/benchmark_three.csv',
        '/Users/c1rush/OS_MAI_Slobodin/build/benchmark_four.csv',
        '/Users/c1rush/OS_MAI_Slobodin/build/benchmark_five.csv'
    ]
    
    # Проверка наличия файлов
    check_files_exist(csv_files)
    
    # Чтение CSV-файлов
    dataframes = read_csv_files(csv_files)
    benchmark_three = dataframes[csv_files[0]]
    benchmark_four = dataframes[csv_files[1]]
    benchmark_five = dataframes[csv_files[2]]
    
    # Настройка стиля с использованием 'ggplot'
    plt.style.use('ggplot')
    plt.rcParams.update({'font.size': 12})
    
    # Визуализация Benchmark Three
    print("Визуализация Benchmark Three: Фрагментация и Фактор Использования")
    plot_benchmark_three_internal_frag(benchmark_three)
    plot_benchmark_three_external_frag(benchmark_three)
    plot_benchmark_three_usage_factor(benchmark_three)
    
    # Визуализация Benchmark Four
    print("Визуализация Benchmark Four: Время Аллокаций")
    plot_benchmark_four_total_alloc_time(benchmark_four)
    plot_benchmark_four_avg_alloc_time(benchmark_four)
    
    # Визуализация Benchmark Five
    print("Визуализация Benchmark Five: Время Деаллокаций")
    plot_benchmark_five_total_free_time(benchmark_five)
    plot_benchmark_five_avg_free_time(benchmark_five)
    
    print("Визуализация завершена. Графики сохранены в текущей директории.")

if __name__ == "__main__":
    main()
