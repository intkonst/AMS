# Стадия сборки
FROM alpine:latest as builder

# Устанавливаем зависимости для сборки
RUN apk update && \
    apk add --no-cache \
    git \
    cmake \
    make \
    g++ \
    postgresql-dev \
    linux-headers

# Клонируем репозиторий
RUN git clone https://github.com/intkonst/AMS.git /AMS
WORKDIR /AMS

# Собираем проект
RUN cmake -B .build && \
    make -C .build

# Стадия рантайма
FROM alpine:latest

# Устанавливаем только необходимые runtime-зависимости
RUN apk update && \
    apk add --no-cache \
    libstdc++ \
    postgresql-libs && \
    # Создаем пользователя для безопасности
    adduser -D appuser

# Копируем ТОЛЬКО папку .bin
COPY --from=builder --chown=appuser /AMS/.bin /app

# Переключаемся на непривилегированного пользователя
USER appuser
WORKDIR /app

# Точка входа (бинарник уже в PATH благодаря расположению в /app)
ENTRYPOINT ["./AMS"]
