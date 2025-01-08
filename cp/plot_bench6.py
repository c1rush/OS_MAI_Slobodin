import pandas as pd
import matplotlib.pyplot as plt

def main():
    csv_file = '/Users/c1rush/OS_MAI_Slobodin/build/benchmark_six_results.csv'
    
    df = pd.read_csv(csv_file)
    
    required_columns = {'MemorySize_MB', 'Allocator', 'AllocCount'}
    if not required_columns.issubset(df.columns):
        missing = required_columns - set(df.columns)
        raise ValueError(f"В файле не хватает колонок: {missing}")
    
    block_df = df[df['Allocator'] == 'BlockAllocator']
    freelist_df = df[df['Allocator'] == 'FreeListAllocator']
    
    block_df = block_df.sort_values(by='MemorySize_MB')
    freelist_df = freelist_df.sort_values(by='MemorySize_MB')
    
    plt.figure(figsize=(8, 6))
    
    plt.plot(block_df['MemorySize_MB'], block_df['AllocCount'], 
             label='BlockAllocator', marker='o', color='blue')
    
    plt.plot(freelist_df['MemorySize_MB'], freelist_df['AllocCount'], 
             label='FreeListAllocator', marker='o', color='orange')
    
    plt.title('Количество выделенных блоков от объёма памяти')
    plt.xlabel('Объём памяти (MB)')
    plt.ylabel('Количество выделений (AllocCount)')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    
    plt.savefig('alloccount_vs_memorysize.png', dpi=150)
    
    plt.show()

if __name__ == "__main__":
    main()
