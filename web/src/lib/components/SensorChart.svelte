<script lang="ts">
    import { onMount, onDestroy } from "svelte";
    import {
        Chart,
        LineController,
        LineElement,
        PointElement,
        LinearScale,
        TimeScale,
        Filler,
        Tooltip,
        type ChartConfiguration,
    } from "chart.js";
    import "chartjs-adapter-date-fns";
    import type { SensorData } from "$lib/ws.svelte";

    Chart.register(
        LineController,
        LineElement,
        PointElement,
        LinearScale,
        TimeScale,
        Filler,
        Tooltip,
    );

    type Props = {
        history: SensorData[];
        field: "temperature" | "humidity";
        color: string;
        glow: string;
        unit: string;
    };

    let { history, field, color, glow, unit }: Props = $props();

    let canvas: HTMLCanvasElement;
    let chart: Chart | null = null;

    function buildConfig(): ChartConfiguration<"line"> {
        return {
            type: "line",
            data: {
                datasets: [
                    {
                        data: [],
                        borderColor: color,
                        borderWidth: 2,
                        pointRadius: 0,
                        pointHoverRadius: 4,
                        pointHoverBackgroundColor: color,
                        fill: true,
                        backgroundColor: glow,
                        tension: 0.4,
                    },
                ],
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                animation: false,
                interaction: { mode: "index", intersect: false },
                plugins: {
                    legend: { display: false },
                    tooltip: {
                        backgroundColor: "#1c2030",
                        borderColor: "#252a3a",
                        borderWidth: 1,
                        titleColor: "#8891aa",
                        bodyColor: "#e8eaf0",
                        padding: 10,
                        callbacks: {
                            label: (ctx) =>
                                ` ${(ctx.parsed.y ?? 0).toFixed(2)} ${unit}`,
                        },
                    },
                },
                scales: {
                    x: {
                        type: "time",
                        time: {
                            unit: "second",
                            displayFormats: { second: "HH:mm:ss" },
                        },
                        grid: { color: "#252a3a" },
                        ticks: {
                            color: "#4a5068",
                            maxTicksLimit: 6,
                            font: { family: "JetBrains Mono", size: 11 },
                        },
                    },
                    y: {
                        grid: { color: "#252a3a" },
                        ticks: {
                            color: "#8891aa",
                            font: { family: "JetBrains Mono", size: 11 },
                            callback: (v) => `${Number(v).toFixed(1)}${unit}`,
                        },
                    },
                },
            },
        };
    }

    onMount(() => {
        chart = new Chart(canvas, buildConfig());
    });

    onDestroy(() => chart?.destroy());

    $effect(() => {
        if (!chart) return;
        const points = history.map((d) => ({
            x: new Date(d.timestamp).getTime(),
            y: d[field],
        }));
        chart.data.datasets[0].data = points;
        chart.update("none");
    });
</script>

<canvas bind:this={canvas}></canvas>
