import { browser } from '$app/environment'

export type SensorData = {
  temperature: number
  humidity: number
  timestamp: string
}

type WsState = {
  connected: boolean
  latest: SensorData | null
  history: SensorData[]
}

const MAX_HISTORY = 60 // 60 seconds of data on chart

function createWsStore() {
  let state = $state<WsState>({
    connected: false,
    latest: null,
    history: [],
  })

  let socket: WebSocket | null = null
  let reconnectTimer: ReturnType<typeof setTimeout> | null = null

  function connect() {
    if (!browser) return

    socket = new WebSocket('ws://localhost:8080/ws')

    socket.onopen = () => {
      state.connected = true
      if (reconnectTimer) {
        clearTimeout(reconnectTimer)
        reconnectTimer = null
      }
    }

    socket.onmessage = (event) => {
      const msg = JSON.parse(event.data)
      if (msg.type === 'history') {
        state.history = msg.data.slice(-MAX_HISTORY)
        if (state.history.length > 0) {
          state.latest = state.history[state.history.length - 1]
        }
      } else if (msg.type === 'live') {
        const data: SensorData = msg.data
        state.latest = data
        state.history = [...state.history.slice(-(MAX_HISTORY - 1)), data]
      }
    }

    socket.onclose = () => {
      state.connected = false
      // auto-reconnect after 2s
      reconnectTimer = setTimeout(connect, 2000)
    }

    socket.onerror = () => {
      socket?.close()
    }
  }

  connect()

  return {
    get connected() { return state.connected },
    get latest() { return state.latest },
    get history() { return state.history },
  }
}

export const ws = createWsStore()
