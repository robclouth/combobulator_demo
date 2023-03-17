import {
  Badge,
  Box,
  Button,
  Card,
  Flex,
  Group,
  Image,
  MantineProvider,
  NavLink,
  ScrollArea,
  SegmentedControl,
  Stack,
  Text,
  TextInput,
  Title,
  useMantineTheme,
} from "@mantine/core";
import { range } from "@mantine/hooks";
import { observer } from "mobx-react";
import { FC, useContext, useEffect, useRef, useState } from "react";
import { TbArrowWaveRightDown, TbSearch } from "react-icons/tb";
import { ParametersContext } from "../juceIntegration/JuceIntegration";
import { EmptySlot } from "./modulators/EmptySlot";
import { LFO } from "./modulators/LFO";

interface Model {
  name: string;
  descriptionShort: string;
  descriptionLong: string;
  tags: string[];
}

const models: Model[] = range(0, 9).map(() => ({
  name: "Test",
  descriptionShort: "A cool model",
  descriptionLong: "This model is so fuckin cool, it'll blow ur fuckin mind",
  tags: ["percussion", "glitch"],
}));

interface ModelCardProps {
  model: Model;
}

export const ModelCard: FC<ModelCardProps> = observer(({ model }) => {
  const { name, descriptionShort, descriptionLong, tags } = model;

  return (
    <Card shadow="sm" padding="sm" radius="md" withBorder miw={200}>
      <Stack mt="xs" mb="xs">
        <Text weight={500}>{name}</Text>
        <Group>
          {tags.map((tag) => (
            <Badge color="pink" variant="light">
              {tag}
            </Badge>
          ))}
        </Group>
      </Stack>

      <Text size="sm" color="dimmed">
        {descriptionShort}
      </Text>
    </Card>
  );
});

interface Props {}

export const Modulators: FC<Props> = observer((props) => {
  const theme = useMantineTheme();
  const parameters = useContext(ParametersContext)!;

  return (
    <Flex style={{ flex: 1 }} direction="column">
      <ScrollArea style={{ flex: "1 1 auto" }} type="scroll" h={0}>
        <Stack p="sm" align="stretch">
          {range(0, 4).map((index) =>
            (parameters as any)[`lfo${index + 1}Enabled`].value ? (
              <LFO index={index} />
            ) : (
              <></>
            )
          )}
          <EmptySlot />
        </Stack>
      </ScrollArea>
    </Flex>
  );
});
