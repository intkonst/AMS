#!/bin/zsh
#!/bin/bash

project_name="AMS"
ex_cmd="./$project_name"

# Сохраняем исходную директорию
ORIGINAL_DIR=$(pwd)

echo "\n===== Starting build process in $(pwd) ====="
echo "Script arguments: $@"

# 1. Генерация проекта CMake
echo "Step 1: Running CMake..."
if ! cmake -B .build; then
    echo "❌ CMake failed"
    cd "$ORIGINAL_DIR"
    return 1 2>/dev/null || exit 1
fi

# 2. Сборка проекта
echo -e "\nStep 2: Building project..."
if ! make -C .build; then
    echo "❌ Build failed"
    cd "$ORIGINAL_DIR"
    return 1 2>/dev/null || exit 1
fi

# 3. Запуск программы с аргументами
echo -e "\nStep 3: Running program with arguments: $@"
if ! (cd .bin && \
     echo -e "Running in: $(pwd)\n\n====$project_name====================================================================\n" && \
     eval "$ex_cmd $@"); then
    echo "\n❌ Execution failed"
    cd "$ORIGINAL_DIR"
    return 1 2>/dev/null || exit 1
fi

# Возвращаемся в исходную директорию
cd "$ORIGINAL_DIR"
echo -e "\n===========================================================================\
\n\nScript 'run.sh': ✅ All done! Returned to: $(pwd)"%                  