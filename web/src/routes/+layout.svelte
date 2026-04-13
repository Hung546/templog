<script lang="ts">
    import "../app.css";

    let { children } = $props();
</script>

<svelte:head>
    <title>TempLog — Sensor Monitor</title>
    <meta
        name="description"
        content="Real-time temperature and humidity monitor" />
</svelte:head>

{@render children()}
